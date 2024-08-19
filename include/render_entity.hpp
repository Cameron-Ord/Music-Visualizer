#ifndef RENDER_ENTITY_HPP
#define RENDER_ENTITY_HPP
#include "font_entity.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

class SDL2Renderer {
public:
  SDL2Renderer();
  ~SDL2Renderer();

  SDL_Renderer** get_renderer();

  void* create_renderer(SDL_Window** w, SDL_Renderer** r);

  void render_clear(SDL_Renderer* r);
  void render_bg(SDL_Renderer* r, SDL_Color* rgba);
  void render_present(SDL_Renderer* r);

  void render_set_directories(std::pair<int, int> sizes, std::vector<Text>* text_vec);
  void render_draw_directories(SDL_Renderer* r, std::vector<Text>* text_vec);

private:
  SDL_Renderer* r;
  size_t        current_dir_draw_index;
  size_t        current_song_draw_index;
  int           directory_draw_limit;
  int           song_draw_limit;
};

#endif
