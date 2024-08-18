#include "../include/sdl2_entity.hpp"
#include <SDL2/SDL.h>

SDL2INTERNAL::SDL2INTERNAL() {
  play_state = 1;
  _window_context = NULL;
  _render_context = NULL;
  _key_input_context = NULL;
  _font_context = NULL;
}

SDL2INTERNAL::~SDL2INTERNAL() {}

int SDL2INTERNAL::initialize_sdl2_events() {
  if (SDL_Init(SDL_INIT_EVENTS) != 0) {
    fprintf(stderr, "SDL2 Could not initialize events -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

int SDL2INTERNAL::initialize_sdl2_video() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL2 Could not initalize video -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

int SDL2INTERNAL::initialize_sdl2_audio() {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "SDL2 could not initialize audio -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

int SDL2INTERNAL::initialize_sdl2_ttf() {
  if (TTF_Init() != 0) {
    fprintf(stderr, "SDL2 TTF could not initialize -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

int SDL2INTERNAL::initialize_sdl2_image() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
    fprintf(stderr, "SDL2 IMG could not initialize -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

SDL2Window *SDL2INTERNAL::get_window_entity() { return _window_context; }
SDL2Renderer *SDL2INTERNAL::get_render_entity() { return _render_context; }
void SDL2INTERNAL::set_play_state(int bool_num) { play_state = bool_num; }
int SDL2INTERNAL::get_play_state() { return play_state; }
SDL2KeyInputs *SDL2INTERNAL::get_key_input_entity() {
  return _key_input_context;
}
SDL2Fonts *SDL2INTERNAL::get_font_entity() { return _font_context; }
