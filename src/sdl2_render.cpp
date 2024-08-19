#include "../include/render_entity.hpp"

SDL2Renderer::SDL2Renderer() {
  r                            = NULL;
  desired_song_draw_limit      = 6;
  desired_directory_draw_limit = 6;
  real_song_draw_limit         = 0;
  real_directory_draw_limit    = 0;
  current_dir_draw_index       = 0;
  current_song_draw_index      = 0;
}

SDL2Renderer::~SDL2Renderer() {}

void
SDL2Renderer::render_set_directories(std::pair<int, int> sizes, std::vector<Text>* text_vec) {
  if (text_vec->size() > desired_directory_draw_limit) {
    size_t position  = current_dir_draw_index + 1;
    size_t remainder = text_vec->size() - position;

    if (remainder > text_vec->size()) {
      real_directory_draw_limit = desired_directory_draw_limit;
    } else {
      real_directory_draw_limit = text_vec->size();
    }
  } else {
    real_directory_draw_limit = text_vec->size();
  }

  int pixel_increment  = sizes.second / (real_directory_draw_limit + 1);
  int pixel_accumulate = pixel_increment;

  for (size_t i = current_dir_draw_index; i < real_directory_draw_limit; i++) {
    if ((*text_vec)[i].is_valid) {
      (*text_vec)[i].rect.x = 25;
      (*text_vec)[i].rect.y = pixel_accumulate;
    }

    pixel_accumulate += pixel_increment;
  }
}

void
SDL2Renderer::render_draw_directories(SDL_Renderer* r, std::vector<Text>* text_vec) {
  for (size_t i = current_dir_draw_index; i < real_directory_draw_limit; i++) {
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
