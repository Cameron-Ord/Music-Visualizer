#ifndef WINDOW_ENTITY_HPP
#define WINDOW_ENTITY_HPP
#include <SDL2/SDL.h>

class SDL2Window {
public:
  SDL2Window();
  SDL_Window *get_window();
  const int *get_border_bool();
  void set_border_bool(int value);
  void set_window(SDL_Window *ptr);

private:
  SDL_Window *w;
  int is_window_bordered;
};
#endif
