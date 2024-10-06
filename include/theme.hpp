#ifndef THEME_HPP
#define THEME_HPP
#include <SDL2/SDL_pixels.h>

typedef enum {
  PRIMARY = 0,
  SECONDARY = 1,
  BACKGROUND = 2,
  TEXT = 3,
  TEXT_BG = 4,
  BG = 5
} STRING_ENUM_REF;

struct HSL_TO_RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

HSL_TO_RGB
phase_hue_effect(const float *phased_hue);

class ProgramThemes {
public:
  ProgramThemes();
  ~ProgramThemes();
  void set_color(SDL_Color to_color, int type);
  const SDL_Color *get_primary();
  const SDL_Color *get_secondary();
  const SDL_Color *get_background();
  const SDL_Color *get_text();
  const SDL_Color *get_textbg();
  float convert_rgba_to_hsl(int TYPE);
  void set_hue_from_rgba(int TYPE);
  const float *get_hue(int TYPE);

private:
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color background;
  SDL_Color text;
  SDL_Color text_bg;
  float primary_hue;
  float secondary_hue;
  float background_hue;
  float text_hue;
  float text_bg_hue;
};

#endif
