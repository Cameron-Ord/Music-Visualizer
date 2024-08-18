#include "../include/sdl2_entity.hpp"

void *SDL2Renderer::create_renderer(SDL_Window **w, SDL_Renderer **r) {
  *r = SDL_CreateRenderer(*w, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!*r) {
    fprintf(stderr, "Could not create renderer! -> %s", SDL_GetError());
    SDL_DestroyWindow(*w);
    return NULL;
  }

  return *r;
}

void SDL2Renderer::render_clear(SDL_Renderer *r) { SDL_RenderClear(r); }
void SDL2Renderer::render_present(SDL_Renderer *r) { SDL_RenderPresent(r); }
void SDL2Renderer::render_bg(SDL_Renderer *r, SDL_Color *rgba) {
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}

SDL2Renderer::SDL2Renderer() { r = NULL; }
SDL_Renderer **SDL2Renderer::get_renderer() { return &r; }
