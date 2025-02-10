#include "../inc/renderer.h"
#include "../inc/main.h"
#include "../inc/window.h"

#include <SDL2/SDL_render.h>

Renderer renderer = {0};

const void *create_renderer(void) {

  const int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  renderer.r = SDL_CreateRenderer(get_window()->w, -1, flags);
  if (!renderer.r) {
    sdl_err(SDL_GetError());
    return NULL;
  }

  return renderer.r;
}

Renderer *get_renderer(void) { return &renderer; }

void destroy_renderer(void) { SDL_DestroyRenderer(renderer.r); }

void render_bg(const SDL_Color *col) {
  SDL_SetRenderDrawColor(renderer.r, col->r, col->g, col->b, col->a);
}

void render_clear(void) { SDL_RenderClear(renderer.r); }

void render_present(void) { SDL_RenderPresent(renderer.r); }
