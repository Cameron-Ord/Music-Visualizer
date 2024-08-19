#ifndef RENDER_ENTITY_HPP
#define RENDER_ENTITY_HPP

#include "font_def.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <vector>

typedef enum {
  DIR_INDEX = 0,
  DIR_LIMITER = 1,
  SONG_INDEX = 2,
  SONG_LIMITER = 3
} LIMITER_ENUM;

class SDL2Renderer {
public:
  SDL2Renderer();
  ~SDL2Renderer();

  SDL_Renderer **get_renderer();
  void *create_renderer(SDL_Window **w, SDL_Renderer **r);
  void render_clear(SDL_Renderer *r);
  void render_bg(SDL_Renderer *r, SDL_Color *rgba);
  void render_present(SDL_Renderer *r);
  void render_set_song_limiter(size_t vec_size);
  void render_set_songs(std::pair<int, int> sizes, std::vector<Text> *text_vec);
  void render_draw_songs(SDL_Renderer *r, std::vector<Text> *text_vec);
  void render_set_directory_limiter(size_t vec_size);
  void render_set_directories(std::pair<int, int> sizes,
                              std::vector<Text> *text_vec);
  void render_draw_directories(SDL_Renderer *r, std::vector<Text> *text_vec);
  void render_set_text_bg(std::pair<int, int> sizes, size_t draw_limit,
                          size_t draw_index, size_t cursor_index,
                          const std::vector<Text> *text_vec);
  void render_draw_text_bg(SDL_Renderer *r, const SDL_Color *rgba);
  size_t get_draw_limit(int LIMITER_ENUM_VALUE);
  size_t get_draw_index(int INDEX_ENUM_VALUE);

private:
  SDL_Renderer *r;
  SDL_Rect text_bg;
  bool drawing_text_bg;

  size_t current_dir_draw_index;
  size_t current_song_draw_index;
  size_t desired_directory_draw_limit;
  size_t desired_song_draw_limit;
  size_t real_directory_draw_limit;
  size_t real_song_draw_limit;
};

#endif
