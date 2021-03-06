#include "ui.h"
#include "../tk/toolkit.h"
#include "ui_text.h"
#include <SDL2/SDL_mixer.h>

#define USERCFG_READ(STR) ini_read(ui->ini_usersettings, "video", STR)
void ui_apply_video_config(ui_t *ui) {
  // emulate a fake fullscreen mode, otherwise the splitscreen stuff
  // won't work. also always run fullscreen inside the wine virtual
  // desktop

  tk_t *tk = ui->tk;
  bool fullscreen =
      (tk->wine ||
       strcmp(ini_read(ui->ini_settings, "video", "fullscreen"), "false"));

  if (fullscreen) {
    if (!tk->wine)
      net_send_to_meta(ui->net, "FULLSCREEN ON", 0);

    if (SDL_GetDesktopDisplayMode(0, &(tk->mode)) != 0)
      exit(printf("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError()));
  } else {
    net_send_to_meta(ui->net, "FULLSCREEN OFF", 0);
    tk->mode.w = atoi(USERCFG_READ("window_width"));
    tk->mode.h = atoi(USERCFG_READ("window_height"));
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,
              ini_read(ui->ini_settings, "video", "menu_upscaling"));
  SDL_SetWindowSize(tk->window, tk->mode.w, tk->mode.h);
  SDL_SetWindowBordered(tk->window, !fullscreen);

  SDL_SetWindowPosition(tk->window, fullscreen ? 0 : SDL_WINDOWPOS_CENTERED,
                        fullscreen ? 0 : SDL_WINDOWPOS_CENTERED);
}
#undef USERCFG_READ

void ui_apply_audio_config(ui_t *ui) {
  int sfx = atoi(ini_read(ui->ini_settings, "audio", "sfx"));
  Mix_Volume(-1, MIX_MAX_VOLUME * (sfx / 9.0));

  int music = atoi(ini_read(ui->ini_settings, "audio", "music"));
  Mix_VolumeMusic(MIX_MAX_VOLUME * (music / 9.0));
}

ui_t *ui_init(tk_t *tk, mmp_t *mmp, net_t *net, sl_t *sl) {
  ui_t *ui = calloc(1, sizeof(ui_t));

  // put all arguments in the struct
  ui->tk = tk;
  ui->net = net;
  ui->net->meta_recv_callback = (void *)ui_callback_for_meta;
  ui->net->native_recv_callback = (void *)ui_callback_for_native;
  ui->net->userdata = (void *)ui;
  ui->mmp = mmp;
  ui->sl = sl;

  ui_init_configs(ui);
  ui->slotmachine =
      (strcmp(ini_read(ui->ini_usersettings, "slotmachine", "enabled"),
              "true") == 0);

  // do update check in background, if the user has enabled it
  if (!strcmp(ini_read(ui->ini_settings, "ui", "update_check_enabled"),
              "true")) {
    ui->chk = chk_init(tk->pref_path, true);
  }

  // letters
  ui->letters = malloc(sizeof(char *) * G2HR_UI_LETTERS_COUNT);
  for (int i = 0; i < G2HR_UI_LETTERS_COUNT; i++) {
    ui->letters[i] = malloc(2);
    ui->letters[i][0] = 'A' + i;
    ui->letters[i][1] = '\0';
  }

  // numbers
  ui->numbers = malloc(sizeof(char *) * G2HR_UI_NUMBERS_COUNT);
  for (int i = 0; i < G2HR_UI_NUMBERS_COUNT; i++) {
    ui->numbers[i] = malloc(2);
    ui->numbers[i][0] = '1' + i;
    ui->numbers[i][1] = '\0';
  }

  // game types
  ui->game_types = malloc(sizeof(char *) * G2HR_UI_GAME_TYPES_COUNT);
  ui->game_types[0] = "FRAGS";
  ui->game_types[1] = "POINTS";
  ui->game_types[2] = "TAG";

  // maps
  ui->maps = malloc(sizeof(char *) * mmp->file_count);
  for (size_t i = 0; i < mmp->file_count; i++)
    ui->maps[i] = ini_read(mmp->files[i]->data, "MapFiles", "Description");

  // player names
  ui->player_names = malloc(sizeof(char *) * GTA2_SAVEGAME_COUNT);
  for (int i = 0; i < GTA2_SAVEGAME_COUNT; i++) {
    ui->player_names[i] = calloc(1, GTA2_PLAYERNAME_MAXLEN + 1);
    ui->player_names[i][0] = 'P';
    ui->player_names[i][1] = 'L';
    ui->player_names[i][2] = 'A';
    ui->player_names[i][3] = 'Y';
    ui->player_names[i][4] = 'E';
    ui->player_names[i][5] = 'R';
    ui->player_names[i][6] = ' ';
    ui->player_names[i][7] = '0' + i;
  }

  // screens
  ui->ready = ui_screen_ready(tk, ui);
  ui->levels = ui_screen_levels(tk, ui);
  ui->credits = ui_screen_credits(tk, ui);
  ui->scores = ui_screen_scores(tk, ui);
  ui->play = ui_screen_play(tk, ui);
  ui->splitscreen = ui_screen_splitscreen(tk, ui);
  ui->opt_audio = ui_screen_opt_audio(tk, ui);
  ui->opt_video = ui_screen_opt_video(tk, ui);
  ui->first_run = ui_screen_first_run(tk, ui);
  ui->options = ui_screen_options(tk, ui);
  ui->main = ui_screen_main_menu(tk, ui);

  // back links
  ui->play->back = ui->main;
  ui->splitscreen->back = ui->main;
  ui->options->back = ui->main;
  ui->opt_audio->back = ui->options;
  ui->opt_video->back = ui->options;
  ui->scores->back = ui->play;
  ui->levels->back = ui->splitscreen;

  // ask if updates should be enabled on first run
  if (strcmp(ini_read(ui->ini_settings, "ui", "update_check_enabled"), "ask"))
    tk->screen_active = ui->main;
  else
    tk->screen_active = ui->first_run;

  if (strcmp(SDL_GetPlatform(), "Windows")) // string does NOT equal
  {
    printf("[menu] not compiled for Windows, showing the UI"
           " directly, without any connection to the meta component."
           " Enjoy debugging!\n");
    ui_show(ui);
  }
  return ui;
}

void ui_show(ui_t *ui) {
  ui_apply_video_config(ui);
  SDL_ShowWindow(ui->tk->window);
  SDL_RaiseWindow(ui->tk->window);

  // apply the video config again, because some options only work
  // when the window is visible
  ui_apply_video_config(ui);

  ui_apply_audio_config(ui);
  sfx_play_song(ui->tk->sfx, SFX_SONG_MAINMENU);
  ui->visible = true;
}

void ui_cleanup(ui_t *ui) {
  for (int i = 0; i < GTA2_SAVEGAME_COUNT; i++)
    free(ui->player_names[i]);
  free(ui->player_names);

  for (int i = 0; i < G2HR_UI_LETTERS_COUNT; i++)
    free(ui->letters[i]);
  free(ui->letters);

  for (int i = 0; i < G2HR_UI_NUMBERS_COUNT; i++)
    free(ui->numbers[i]);
  free(ui->numbers);

  free(ui->maps);
  free(ui->game_types);

  ui_cleanup_configs(ui);
  chk_cleanup(ui->chk);

  free(ui);
}
