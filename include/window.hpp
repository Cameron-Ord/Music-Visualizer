#ifndef WINDOW_ENTITY_HPP
#define WINDOW_ENTITY_HPP
#include <SDL2/SDL.h>

class SDL2Window {
public:
  SDL2Window();
  void *create_window(SDL_Window **w);
  SDL_Window **get_window();
  const int *get_border_bool();
  void set_border_bool(int value);

private:
  SDL_Window *w;
  int is_window_bordered;
};
#endif
