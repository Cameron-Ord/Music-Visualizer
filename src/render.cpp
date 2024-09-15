#include "../include/render_entity.hpp"

SDL2Renderer::SDL2Renderer() {
  r = NULL;
  desired_song_draw_limit = 12;
  desired_directory_draw_limit = 6;
  real_song_draw_limit = 0;
  real_directory_draw_limit = 0;
  current_dir_draw_index = 0;
  current_song_draw_index = 0;
}

SDL2Renderer::~SDL2Renderer() {}

void SDL2Renderer::reset_vector_positions() {
  real_directory_draw_limit = 0;
  real_song_draw_limit = 0;
  current_dir_draw_index = 0;
  current_song_draw_index = 0;
}

void SDL2Renderer::set_draw_limits(int h) {
  if (h < 400) {
    desired_song_draw_limit = 4;
    desired_directory_draw_limit = 2;
    return;
  } else if (h > 400 && h < 800) {
    desired_song_draw_limit = 6;
    desired_directory_draw_limit = 4;
  } else if (h > 800 && h < 1280) {
    desired_song_draw_limit = 8;
    desired_directory_draw_limit = 6;
  } else if (h > 1280) {
    desired_directory_draw_limit = 8;
    desired_song_draw_limit = 12;
  }
}

size_t SDL2Renderer::get_draw_limit(int LIMITER_ENUM_VALUE) {
  switch (LIMITER_ENUM_VALUE) {
  case DIR_LIMITER: {
    return real_directory_draw_limit;
  }
  case SONG_LIMITER: {
    return real_song_draw_limit;
  }
  default: {
    return 0;
  }
  }
}

void SDL2Renderer::render_draw_bars(size_t *len, float *smear, float *smooth,
                                    int *win_height, int *win_width,
                                    SDL_Color *prim, SDL_Color *sec,
                                    SDL_Renderer *r) {
  int cell_width = *win_width / *len;
  int h = *win_height;

  for (size_t i = 0; i < *len; ++i) {
    float start = smear[i];
    float end = smooth[i];

    int end_x_pos = (i * (int)(cell_width + cell_width / 2));
    int end_y_pos = h - (end * h);
    int end_bar_height = end * h;
    SDL_Rect sample_end = {end_x_pos, end_y_pos, cell_width, end_bar_height};

    SDL_SetRenderDrawColor(r, prim->r, prim->g, prim->b, prim->a);
    SDL_RenderFillRect(r, &sample_end);

    int start_x_pos = (i * (int)(cell_width + cell_width / 2));
    int start_y_pos = h - (start * h);

    if (end_y_pos > start_y_pos) {
      SDL_Rect sample_start = {start_x_pos, start_y_pos, cell_width,
                               end_y_pos - start_y_pos};
      SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
      SDL_RenderFillRect(r, &sample_start);
    }
  }
}

size_t SDL2Renderer::get_draw_index(int INDEX_ENUM_VALUE) {
  switch (INDEX_ENUM_VALUE) {
  case DIR_INDEX: {
    return current_dir_draw_index;
  }
  case SONG_INDEX: {
    return current_song_draw_index;
  }
  default: {
    return 0;
  }
  }
}

void SDL2Renderer::render_set_song_limiter(size_t vec_size) {
  if (vec_size < desired_song_draw_limit) {
    real_song_draw_limit = vec_size;
    return;
  }

  size_t position = current_song_draw_index + 1;
  size_t remainder = vec_size - position;

  if (remainder > desired_song_draw_limit) {
    real_song_draw_limit = desired_song_draw_limit;
  } else {
    real_song_draw_limit = remainder;
  }
}

void SDL2Renderer::render_set_songs(std::pair<int, int> sizes,
                                    std::vector<Text> *text_vec) {
  int pixel_increment = sizes.second / (real_song_draw_limit + 1);
  int pixel_accumulate = pixel_increment;

  size_t draw_index = current_song_draw_index;
  for (size_t i = 0; i < real_song_draw_limit; i++) {
    if ((*text_vec)[draw_index].is_valid) {
      (*text_vec)[draw_index].rect.x = 25;
      (*text_vec)[draw_index].rect.y = pixel_accumulate;
    }

    pixel_accumulate += pixel_increment;
    draw_index++;
  }
}

void SDL2Renderer::render_draw_songs(SDL_Renderer *r,
                                     std::vector<Text> *text_vec) {
  for (size_t i = current_song_draw_index; i < real_song_draw_limit; i++) {
    if ((*text_vec)[i].is_valid) {
      SDL_RenderCopy(r, (*text_vec)[i].tex, NULL, &(*text_vec)[i].rect);
    }
  }
}

void SDL2Renderer::render_set_directory_limiter(size_t vec_size) {
  if (vec_size < desired_directory_draw_limit) {
    real_directory_draw_limit = vec_size;
    return;
  }

  size_t position = current_dir_draw_index + 1;
  size_t remainder = vec_size - position;

  if (remainder > desired_directory_draw_limit) {
    real_directory_draw_limit = desired_directory_draw_limit;
  } else {
    real_directory_draw_limit = remainder;
  }
}

void SDL2Renderer::render_set_directories(std::pair<int, int> sizes,
                                          std::vector<Text> *text_vec) {
  int pixel_increment = sizes.second / (real_directory_draw_limit + 1);
  int pixel_accumulate = pixel_increment;

  size_t draw_index = current_dir_draw_index;
  for (size_t i = 0; i < real_directory_draw_limit; i++) {
    if ((*text_vec)[draw_index].is_valid) {
      (*text_vec)[draw_index].rect.x = 25;
      (*text_vec)[draw_index].rect.y = pixel_accumulate;
    }

    pixel_accumulate += pixel_increment;
    draw_index++;
  }
}

void SDL2Renderer::render_draw_directories(SDL_Renderer *r,
                                           std::vector<Text> *text_vec) {
  for (size_t i = current_dir_draw_index; i < real_directory_draw_limit; i++) {
    if ((*text_vec)[i].is_valid) {
      SDL_RenderCopy(r, (*text_vec)[i].tex, NULL, &(*text_vec)[i].rect);
    }
  }
}

void SDL2Renderer::render_set_text_bg(std::pair<int, int> sizes,
                                      size_t draw_limit, size_t draw_index,
                                      size_t cursor_index,
                                      const std::vector<Text> *text_vec) {
  int pixel_increment = sizes.second / (draw_limit + 1);
  int pixel_accumulate = pixel_increment;

  drawing_text_bg = false;

  for (size_t i = 0; i < draw_limit; i++) {
    if (i == cursor_index) {
      text_bg.w = (*text_vec)[i + draw_index].width + 10;
      text_bg.h = (*text_vec)[i + draw_index].height + 10;
      text_bg.x = 25 - 5;
      text_bg.y = pixel_accumulate - 5;
      drawing_text_bg = true;
    }

    pixel_accumulate += pixel_increment;
  }
}

void SDL2Renderer::render_draw_text_bg(SDL_Renderer *r, const SDL_Color *rgba) {
  if (drawing_text_bg) {
    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a / 2);
    SDL_RenderFillRect(r, &text_bg);
  }
}

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

SDL_Renderer **SDL2Renderer::get_renderer() { return &r; }
