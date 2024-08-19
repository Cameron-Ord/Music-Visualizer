#include "../include/render_entity.hpp"

SDL2Renderer::SDL2Renderer() { r = NULL; }

SDL2Renderer::~SDL2Renderer() {}

void
SDL2Renderer::render_set_directories(std::pair<int, int> sizes, std::vector<Text>* text_vec) {
  const int eight_tenths_w = 0.8 * sizes.first;

  for (size_t i = 0; i < text_vec->size(); i++) {
    if ((*text_vec)[i].is_valid) {
      (*text_vec)[i].rect.x = eight_tenths_w;
    }
  }
}

void
SDL2Renderer::render_draw_directories(SDL_Renderer* r, std::vector<Text>* text_vec) {
  for (size_t i = 0; i < text_vec->size(); i++) {
    if ((*text_vec)[i].is_valid) {
      SDL_RenderCopy(r, (*text_vec)[i].tex, NULL, &(*text_vec)[i].rect);
    }
  }
}

void*
SDL2Renderer::create_renderer(SDL_Window** w, SDL_Renderer** r) {
  *r = SDL_CreateRenderer(*w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!*r) {
    fprintf(stderr, "Could not create renderer! -> %s", SDL_GetError());
    SDL_DestroyWindow(*w);
    return NULL;
  }

  return *r;
}

void
SDL2Renderer::render_clear(SDL_Renderer* r) {
  SDL_RenderClear(r);
}
void
SDL2Renderer::render_present(SDL_Renderer* r) {
  SDL_RenderPresent(r);
}
void
SDL2Renderer::render_bg(SDL_Renderer* r, SDL_Color* rgba) {
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}

SDL_Renderer**
SDL2Renderer::get_renderer() {
  return &r;
}
