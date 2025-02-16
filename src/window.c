#include "../inc/window.h"
#include "../inc/main.h"
#include <SDL2/SDL.h>

Window window = {0};

const void *create_window(void) {
  const int pos_flag = SDL_WINDOWPOS_CENTERED;
  const int window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;

  window.w = SDL_CreateWindow("Music Visualizer", pos_flag, pos_flag, 600, 400,
                              window_flags);
  if (!window.w) {
    sdl_err(SDL_GetError());
    return NULL;
  }

  window.width = 600, window.height = 400;

  return window.w;
}

Window *get_window(void) { return &window; }
void close_window(void) { SDL_DestroyWindow(window.w); }
void win_push_update(void) {
  SDL_GetWindowSize(window.w, &window.width, &window.height);
}
