#ifndef WINDOW_ENTITY_HPP
#define WINDOW_ENTITY_HPP
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>

class SDL2Window
{
  public:
    SDL2Window();
    void *create_window(SDL_Window **w);
    SDL_Window **get_window();

  private:
    SDL_Window *w;
};
#endif
