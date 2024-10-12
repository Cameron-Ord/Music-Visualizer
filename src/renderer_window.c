#include "main.h"

void render_bg(void) {
  scc(SDL_SetRenderDrawColor(rend.r, vis.background.r, vis.background.g,
                             vis.background.b, vis.background.a));
}

void render_clear(void) { scc(SDL_RenderClear(rend.r)); }

void render_present(void) { SDL_RenderPresent(rend.r); }
