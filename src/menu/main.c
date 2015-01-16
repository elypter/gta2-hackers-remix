#include "toolkit/toolkit.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  const char *tgas[] = {"1",      "1_play",   "1_options",
                        "1_quit", "3_tables", "credits" /* ... */};

  // Initialize SDL2, the toolkit and the GTA2 backgrounds
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    exit(printf("SDL_ERROR: %s\n", SDL_GetError()));
  tk_t *tk = tk_init("G2HR");
  for (int i = 0; i < sizeof(tgas) / sizeof(char *); i++)
    tk_init_gta2_background(tk, tgas[i]);

  tk_control_t *ctrl;
  tk_screen_t *main = tk_screen_create(tk, NULL);

  ctrl = tk_control_add(main, 0x00, NULL, "Start");
  tk_control_setbg(tk, ctrl, NULL, "1_play", "1");

  ctrl = tk_control_add(main, 0x00, NULL, "Options");
  tk_control_setbg(tk, ctrl, NULL, "1_options", "1");

  ctrl = tk_control_add(main, 0x00, NULL, "Quit");
  tk_control_setbg(tk, ctrl, NULL, "1_quit", "1");

  tk->screen = main;

  while (1) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if (event.type == SDL_QUIT)
      break;
    tk_frame(tk, &event);
  }

  tk_cleanup(tk);
  SDL_Quit();

  printf("Cleaned up!\n");
  return 0;
}
