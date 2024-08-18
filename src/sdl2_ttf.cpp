#include "../include/sdl2_entity.hpp"

int SDL2Fonts::open_font() {
  font = TTF_OpenFont(font_path.c_str(), font_size);
  if (font == NULL) {
    fprintf(stderr, "Could not open TTF font -> %s\n", SDL_GetError());
    return 0;
  }
  return 1;
}

SDL2Fonts::SDL2Fonts(SDL_Color text_color) {
  font = NULL;
  font_path = "dogicapixel.ttf";
  font_size = 16;
  font_color = text_color;
}

SDL2Fonts::~SDL2Fonts() {}
