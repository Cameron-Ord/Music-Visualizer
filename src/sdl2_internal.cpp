#include "../include/sdl2_entity.hpp"
#include <SDL2/SDL.h>

SDL2INTERNAL::SDL2INTERNAL() {
  play_state         = 1;
  _window_context    = NULL;
  _render_context    = NULL;
  _key_input_context = NULL;
  _font_context      = NULL;
  _themes            = NULL;
  _path              = NULL;
  _files             = NULL;
}

SDL2INTERNAL::~SDL2INTERNAL() {}

bool
SDL2INTERNAL::initialize_sdl2_events() {
  if (SDL_Init(SDL_INIT_EVENTS) != 0) {
    fprintf(stderr, "SDL2 Could not initialize events -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

bool
SDL2INTERNAL::initialize_sdl2_video() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL2 Could not initalize video -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

bool
SDL2INTERNAL::initialize_sdl2_audio() {
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "SDL2 could not initialize audio -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

bool
SDL2INTERNAL::initialize_sdl2_ttf() {
  if (TTF_Init() != 0) {
    fprintf(stderr, "SDL2 TTF could not initialize -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

bool
SDL2INTERNAL::initialize_sdl2_image() {
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
    fprintf(stderr, "SDL2 IMG could not initialize -> %s\n", SDL_GetError());
    return false;
  }
  return true;
}

void
SDL2INTERNAL::set_play_state(bool state) {
  play_state = state;
}

bool
SDL2INTERNAL::get_play_state() {
  return play_state;
}

void
SDL2INTERNAL::set_entity(void* void_class_ptr, int CLASS_ENUM_VALUE) {
  switch (CLASS_ENUM_VALUE) {
  case WINDOW: {
    _window_context = (SDL2Window*)void_class_ptr;
    return;
  }

  case FONT: {
    _font_context = (SDL2Fonts*)void_class_ptr;
    return;
  }

  case RENDERER: {
    _render_context = (SDL2Renderer*)void_class_ptr;
    return;
  }

  case KEY_INPUT: {
    _key_input_context = (SDL2KeyInputs*)void_class_ptr;
    return;
  }

  case THEMES: {
    _themes = (ProgramThemes*)void_class_ptr;
    return;
  }

  case FILES: {
    _files = (ProgramFiles*)void_class_ptr;
    return;
  }

  case PATHS: {
    _path = (ProgramPath*)void_class_ptr;
    return;
  }

  default: {
    return;
  }
  }
}

void*
SDL2INTERNAL::get_entity(int CLASS_ENUM_VALUE) {
  switch (CLASS_ENUM_VALUE) {
  case WINDOW: {
    return _window_context;
  }

  case FONT: {
    return _font_context;
  }

  case RENDERER: {
    return _render_context;
  }

  case KEY_INPUT: {
    return _key_input_context;
  }

  case THEMES: {
    return _themes;
  }

  case FILES: {
    return _files;
  }

  case PATHS: {
    return _path;
  }

  default: {
    return NULL;
  }
  }
}
