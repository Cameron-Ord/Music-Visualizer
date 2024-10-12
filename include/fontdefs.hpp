#ifndef FONTDEFS_HPP
#define FONTDEFS_HPP
#include <SDL2/SDL_render.h>
#include <iostream>

struct Text {
  SDL_Texture *tex;
  SDL_Surface *surf;
  SDL_Rect rect;
  int width;
  int height;
  size_t id;
  bool is_valid;
  std::string name;
};

struct SettingTextInt {
  Text setting_text;
  const int *setting_value_ptr;
  SDL_Rect setting_value_rect;
};

struct SettingTextFloat {
  Text setting_text;
  const float *setting_value_ptr;
  SDL_Rect setting_value_rect;
};

typedef struct Text Text;

#endif
