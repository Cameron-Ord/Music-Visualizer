#include "../include/enumdefs.hpp"
#include "../include/globals.hpp"
#include "../include/macdefs.hpp"
#include "../include/particles.hpp"
#include <SDL2/SDL_render.h>

SDL2Renderer::SDL2Renderer() {
  r = NULL;
  bar_end_coords.clear();
  bar_start_coords.clear();
  font_draw_limit = 0;
  set_length = 0;
  setting_render_mode = FLOATS;
}

SDL2Renderer::~SDL2Renderer() {}

size_t *SDL2Renderer::get_particle_buffer_size(){
  return &particle_buffer_size;
}

Particle** SDL2Renderer::get_particle_buffer(){
  return particle_buffer;
}

const std::vector<Coordinates>* SDL2Renderer::get_start_cords_buf(){
  return &bar_start_coords;
}

void SDL2Renderer::allocate_particle_buffer(){
  particle_buffer = (Particle**)malloc(sizeof(Particle*) * 256);
  for(int i = 0; i < 256; i++){
    particle_buffer[i] = NULL;
  }

  particle_buffer_size = 256;
}

void SDL2Renderer::render_state_handler(ProgramThemes *themes, FData *ftdata,
                                        FBuffers *ftbufs) {
  std::vector<Text> *dir =
      fonts.retrieve_indexed_dir_textvector(*key.get_vdir_index());
  std::vector<Text> *song =
      fonts.retrieve_indexed_song_textvector(*key.get_vsong_index());
  const WIN_SIZE *win_size = sdl2.get_stored_window_size();
  const size_t *dir_vcursor = key.get_vdir_cursor_index();
  const size_t *song_vcursor = key.get_vsong_cursor_index();

  switch (sdl2.get_current_user_state()) {
  case AT_DIRECTORIES: {
    if (fonts.get_dir_vec_size() > 0) {
      render_set_text(win_size, dir);
      render_draw_text(dir);
      render_set_text_bg(win_size, dir, dir_vcursor);
      render_draw_text_bg(themes->get_textbg());
    }
    break;
  }

  case AT_SONGS: {
    if (fonts.get_song_vec_size() > 0) {
      render_set_text(win_size, song);
      render_draw_text(song);
      render_set_text_bg(win_size, song, song_vcursor);
      render_draw_text_bg(themes->get_textbg());
    }
    break;
  }

  case LISTENING: {
    render_set_bars(&ftdata->output_len, &win_size->HEIGHT, &win_size->WIDTH,
                    ftbufs->smear, ftbufs->smoothed, ftbufs->processed_phases);
    render_draw_bars(themes->get_primary(), themes->get_secondary(),
                     ftbufs->processed_phases);
    std::cout << "HELLO" << std::endl;

    render_draw_particle(rend.get_particle_buffer(), rend.get_particle_buffer_size(), &ftdata->output_len);
    break;
  }

  case AT_SETTINGS: {
    switch (*rend.get_setting_render_mode()) {
    default: {
      break;
    }

    case FLOATS: {
      render_draw_float_settings(fonts.get_float_settings_vec(), win_size,
                                 themes->get_primary(), themes->get_primary(),
                                 key.get_settings_cursor());
      break;
    }

    case INTS: {
      render_draw_int_settings(fonts.get_int_settings_vec(), win_size,
                               themes->get_primary(), themes->get_primary(),
                               key.get_settings_cursor());
      break;
    }
    }
  }

  default: {
    break;
  }
  }
}

const int *SDL2Renderer::get_setting_render_mode() {
  return &setting_render_mode;
}

void SDL2Renderer::set_setting_render_mode(int MODE) {
  setting_render_mode = MODE;
}

void SDL2Renderer::render_draw_int_settings(
    std::vector<SettingTextInt> *int_sett, const WIN_SIZE *window_sizing,
    const SDL_Color *rgba, const SDL_Color *sec, const size_t *cursor) {
  const size_t size = int_sett->size();
  const int win_width = window_sizing->WIDTH;
  const int win_height = window_sizing->HEIGHT;

  int pixel_increment = win_height / (static_cast<int>(size) + 1);
  int pixel_accumulate = pixel_increment;

  for (size_t i = 0; i < size; i++) {
    SDL_Rect *setting_rect = &(*int_sett)[i].setting_text.rect;
    SDL_Rect *setting_val_rect = &(*int_sett)[i].setting_value_rect;
    SDL_Texture *texture = (*int_sett)[i].setting_text.tex;

    const int *setting_value = (*int_sett)[i].setting_value_ptr;
    setting_rect->x =
        static_cast<int>((win_width * 0.5)) - (setting_rect->w / 2);
    setting_rect->y = pixel_accumulate - (setting_rect->h / 2);

    const float normalized =
        static_cast<float>(*setting_value) / INT_SETTING_MAX;
    setting_val_rect->x = static_cast<int>(normalized * win_width) - (16 / 2);
    setting_val_rect->y =
        (pixel_accumulate + (setting_rect->h + 20)) - (16 / 2);
    setting_val_rect->w = 16;
    setting_val_rect->h = 16;

    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
    SDL_RenderCopy(r, texture, NULL, setting_rect);
    SDL_RenderFillRect(r, setting_val_rect);

    if (i == *cursor) {
      SDL_Rect rect_cpy = *setting_val_rect;

      rect_cpy.h += 10;
      rect_cpy.w += 10;
      rect_cpy.x = static_cast<int>(normalized * win_width) - (26 / 2);
      rect_cpy.y = (pixel_accumulate + (setting_rect->h + 20)) - (26 / 2);

      SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
      SDL_RenderFillRect(r, &rect_cpy);
    }

    pixel_accumulate += pixel_increment;
  }
}

void SDL2Renderer::render_draw_float_settings(
    std::vector<SettingTextFloat> *float_sett, const WIN_SIZE *window_sizing,
    const SDL_Color *rgba, const SDL_Color *sec, const size_t *cursor) {

  const size_t size = float_sett->size();
  const int win_width = window_sizing->WIDTH;
  const int win_height = window_sizing->HEIGHT;

  int pixel_increment = win_height / (static_cast<int>(size) + 1);
  int pixel_accumulate = pixel_increment;

  for (size_t i = 0; i < size; i++) {
    SDL_Rect *setting_rect = &(*float_sett)[i].setting_text.rect;
    SDL_Rect *setting_val_rect = &(*float_sett)[i].setting_value_rect;
    SDL_Texture *texture = (*float_sett)[i].setting_text.tex;

    const float *setting_value = (*float_sett)[i].setting_value_ptr;
    setting_rect->x =
        static_cast<int>((win_width * 0.5)) - (setting_rect->w / 2);
    setting_rect->y = pixel_accumulate - (setting_rect->h / 2);

    setting_val_rect->x =
        static_cast<int>(*setting_value * win_width) - (16 / 2);
    setting_val_rect->y =
        (pixel_accumulate + (setting_rect->h + 20)) - (16 / 2);
    setting_val_rect->w = 16;
    setting_val_rect->h = 16;

    SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
    SDL_RenderCopy(r, texture, NULL, setting_rect);
    SDL_RenderFillRect(r, setting_val_rect);

    if (i == *cursor) {
      SDL_Rect rect_cpy = *setting_val_rect;

      rect_cpy.h += 10;
      rect_cpy.w += 10;
      rect_cpy.x = static_cast<int>(*setting_value * win_width) - (26 / 2);
      rect_cpy.y = (pixel_accumulate + (setting_rect->h + 20)) - (26 / 2);

      SDL_SetRenderDrawColor(r, sec->r, sec->g, sec->b, sec->a);
      SDL_RenderFillRect(r, &rect_cpy);
    }

    pixel_accumulate += pixel_increment;
  }
}

void SDL2Renderer::reset_vector_positions() {}

void SDL2Renderer::set_font_draw_limit(int h) {
  font_draw_limit = std::min(24, std::max(1, (h - 100) / 32));
}

const size_t *SDL2Renderer::get_font_draw_limit() { return &font_draw_limit; }

void *SDL2Renderer::create_renderer(SDL_Window **w, SDL_Renderer **r) {
  *r = SDL_CreateRenderer(*w, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!*r) {
    fprintf(
        stderr,
        "Could not create renderer! -> %s : Fallback to Software Rasterization",
        SDL_GetError());

    *r = SDL_CreateRenderer(*w, -1, SDL_RENDERER_SOFTWARE);
  }

  if (!*r) {
    std::cerr << "Could not create CPU rasterization renderer! -> "
              << SDL_GetError() << std::endl;
    SDL_DestroyWindow(*w);
    return NULL;
  }

  return *r;
}

void SDL2Renderer::render_clear() { SDL_RenderClear(r); }
void SDL2Renderer::render_present() { SDL_RenderPresent(r); }
void SDL2Renderer::render_bg(const SDL_Color *rgba) {
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}
SDL_Renderer **SDL2Renderer::get_renderer() { return &r; }
