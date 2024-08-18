#ifndef RENDER_ENTITY_H
#define RENDER_ENTITY_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

class SDL2Renderer {
public:
  SDL2Renderer();
  void *create_renderer(SDL_Window **w, SDL_Renderer **r);
  SDL_Renderer **get_renderer();
  void render_clear(SDL_Renderer *r);
  void render_bg(SDL_Renderer *r, SDL_Color *rgba);
  void render_present(SDL_Renderer *r);

private:
  SDL_Renderer *r;
};

#endif
