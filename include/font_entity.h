#ifndef SDL2_TTF_H
#define SDL2_TTF_H

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

class SDL2Fonts {
public:
  SDL2Fonts(SDL_Color text_color);
  ~SDL2Fonts();
  int open_font();

private:
  std::string font_path;
  int font_size;
  TTF_Font *font;
  SDL_Color font_color;
};

#endif
