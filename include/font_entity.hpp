#ifndef SDL2_TTF_HPP
#define SDL2_TTF_HPP

#include "file_def.hpp"
#include "font_def.hpp"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

class SDL2Fonts {
public:
  SDL2Fonts();
  ~SDL2Fonts();
  SDL_Surface *create_text_surface(TTF_Font *font, const SDL_Color color,
                                   const std::string text);
  SDL_Texture *create_text_texture(SDL_Renderer *r, SDL_Surface *surf);
  void create_dir_text(const std::vector<Directory> d, SDL_Renderer *r,
                       const SDL_Color color, TTF_Font *font);
  void create_file_text(const std::vector<Files> f, SDL_Renderer *r,
                        const SDL_Color color, TTF_Font *font);
  void destroy_file_text(std::vector<Text> *file_vec);
  void destroy_dir_text(std::vector<Text> *dir_vec);
  SDL_Surface *destroy_text_surface(SDL_Surface *ptr);
  SDL_Texture *destroy_text_texture(SDL_Texture *ptr);
  bool open_font();
  Text create_text(const std::string text, TTF_Font *font, SDL_Renderer *r,
                   const int text_id, const SDL_Color color);
  TTF_Font *get_font_ptr();

  std::vector<Text> *get_dir_vec();
  std::vector<Text> *get_song_vec();

private:
  std::vector<Text> dir_text_vec;
  std::vector<Text> song_text_vec;
  std::string font_path;
  int font_size;
  size_t character_limit;
  TTF_Font *font;
};

#endif
