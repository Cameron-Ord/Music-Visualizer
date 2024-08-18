#ifndef SDL2_TTF_H
#define SDL2_TTF_H

#include "files.hpp"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

struct Text {
  SDL_Texture *tex;
  SDL_Surface *surf;
  SDL_Rect rect;
  int id;
};

typedef Text Text;

class SDL2Fonts {
public:
  SDL2Fonts();
  ~SDL2Fonts();
  SDL_Surface *create_text_surface(TTF_Font *font, SDL_Color color,
                                   std::string text);
  SDL_Texture *create_text_texture(SDL_Renderer *r, SDL_Surface *surf);

  int create_dir_text(std::vector<Directory> d, SDL_Renderer *r,
                      SDL_Color color);
  int create_file_text(std::vector<Files> f, SDL_Renderer *r, SDL_Color color);
  void destroy_dir_text();
  void destroy_file_text();
  void destroy_text_surface();
  void destroy_text_texture();
  int open_font();
  Text create_text(std::string text, SDL_Renderer *r, SDL_Color color);

private:
  std::string font_path;
  int font_size;
  TTF_Font *font;
};

#endif
