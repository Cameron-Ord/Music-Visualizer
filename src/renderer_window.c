#include "renderer.h"

void render_bg(const SDL_Color *col, SDL_Renderer *r) {
  SDL_SetRenderDrawColor(r, col->r, col->g, col->b, col->a);
}

void render_clear(SDL_Renderer *r) { SDL_RenderClear(r); }

void render_present(SDL_Renderer *r) { SDL_RenderPresent(r); }
