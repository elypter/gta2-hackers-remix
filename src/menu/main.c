#include "bg/background.h"
#include "chk/chk.h"
#include "gxt/gxt.h"
#include "mmp/mmp.h"
#include "net/menu_net.h"
#include "rpc/rpc.h"
#include "sfx/sfx.h"
#include "sl/sl.h"
#include "sty/sty.h"
#include "tk/toolkit.h"
#include "ui/ui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define G2HR_MENU_FPS 20

/*
        NOTE: the net sub-component connects to the native component on
                startup. When it fails, it shows a message box and quits.
*/

int main(int argc, char *argv[]) {
  if (argc != 2 && !strcmp(SDL_GetPlatform(), "Windows"))
    exit(SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR, "G2HR",
        "'menu.exe' gets startet by 'native.exe' (native.bin on"
        " Linux). Start that instead!",
        NULL));

  // FIXME: attach gxt to ui instead of tk!

  // init all
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    exit(printf("SDL ERROR: %s\n", SDL_GetError()));
  if (SDLNet_Init() == -1)
    exit(printf("SDLNet ERROR: %s\n", SDLNet_GetError()));
  IMG_Init(IMG_INIT_PNG);
  net_t *net = net_init();

  // load configs
  char *pref_path = SDL_GetPrefPath("bytewerk", "G2HR");
  chk_t *chk = NULL; // chk_init(pref_path, true);
  sl_t *sl = sl_init("data/screen_layouts.cfg");

  // load GTA2 files
  gxt_t *gxt = gxt_load("GTA2/data/e.gxt");
  sty_t *fsty = sty_load("GTA2/data/fstyle.sty");
  sfx_t *sfx = sfx_init();
  mmp_t *mmp = mmp_init("GTA2/data");
  bg_t *bg = bg_init();

  tk_t *tk = tk_init(gxt, fsty, sfx, bg, pref_path, "G2HR");
  ui_t *ui = ui_init(tk, mmp, net, sl);

  // run registry path changer
  rpc_init(pref_path);

  // start the meta component
  if (!strcmp(SDL_GetPlatform(), "Windows")) {
    int buffer_size = 30;
    char *buffer = malloc(buffer_size + 1);
    snprintf(buffer, buffer_size, "start bin/meta.exe %i", net->server_port);
    system(buffer);
    free(buffer);
  } else
    printf("NOTE: You have not compiled the menu for win32, so the 'meta' "
           "component will not be started. The commands that *would* be sent "
           "to it are shown in the terminal anyway. Enjoy debugging :)\n");

  // main loop
  uint32_t previous = 0;
  while (!tk->quit) {
    SDL_Event event;
    if (SDL_WaitEventTimeout(&event, 100 / G2HR_MENU_FPS)) {
      if (event.type == SDL_QUIT)
        break;
      tk_frame(tk, &event);
    } else {
      uint32_t now = SDL_GetTicks();
      if (now - previous >= 1000 / G2HR_MENU_FPS) {
        previous = now;
        tk_frame(tk, NULL);
      }
    }

    if (!net_frame(net))
      tk->quit = 1;
  }

  // cleanup all
  ui_cleanup(ui);
  tk_cleanup(tk);
  sl_cleanup(sl);
  chk_cleanup(chk);
  free(pref_path);
  mmp_cleanup(mmp);
  bg_cleanup(bg);
  sfx_cleanup(sfx);
  sty_cleanup(fsty);
  gxt_cleanup(gxt);
  net_cleanup(net);
  IMG_Quit();
  SDLNet_Quit();
  SDL_Quit();

  printf("cleaned up!\n");
  return 0;
}
