#include "toolkit.h"
#include <stdio.h>

void up(tk_t *tk, tk_screen_t *screen, tk_el_t *selected, tk_el_t *first) {
  tk_el_t *listpos = first;

  // first entry -> get the last one!
  if (selected == listpos)
    while (listpos->next)
      listpos = listpos->next;
  else
    while (listpos && listpos->next && listpos->next != selected)
      listpos = listpos->next;

  screen->el_selected = listpos;

  if (screen->el_selected->flags & TK_EL_FLAG_DISABLED ||
      !(screen->el_selected->flags & TK_EL_FLAG_SELECTABLE))
    up(tk, screen, screen->el_selected, first);

  sfx_play_sample(tk->sfx, SFX_FSTYLE_UP_DOWN);
}

void down(tk_t *tk, tk_screen_t *screen, tk_el_t *selected, tk_el_t *first) {
  screen->el_selected = selected->next ? selected->next : first;

  if (screen->el_selected->flags & TK_EL_FLAG_DISABLED ||
      !(screen->el_selected->flags & TK_EL_FLAG_SELECTABLE))
    down(tk, screen, screen->el_selected, first);

  sfx_play_sample(tk->sfx, SFX_FSTYLE_UP_DOWN);
}

void back(tk_t *tk, tk_screen_t *screen) {
  if (!screen->back)
    return;
  sfx_play_sample(tk->sfx, SFX_FSTYLE_BACK);

  tk->screen_active = screen->back;
}

void tk_screen_actionfunc(tk_t *tk, tk_el_t *el, tk_el_t *el_selected,
                          tk_action_t action, SDL_Keycode key) {
  tk_screen_t *screen = tk->screen_active;
  tk_el_t *first = screen->el_content_container
                       ? screen->el_content_container->sub
                       : screen->el.sub;

  if (el_selected) {
    if (action == TK_ACTION_UP)
      up(tk, screen, el_selected, first);
    if (action == TK_ACTION_DOWN)
      down(tk, screen, el_selected, first);
    if (action == TK_ACTION_BACKSPACE || action == TK_ACTION_ESC)
      back(tk, screen);
  }

  if (screen->actionfunc) {
    void (*actionfunc)(tk_t *, tk_el_t *, tk_el_t *, tk_action_t,
                       SDL_Keycode key) = screen->actionfunc;
    if (actionfunc)
      actionfunc(tk, el, el_selected, action, key);
  }
}
