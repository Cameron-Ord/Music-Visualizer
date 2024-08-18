#ifndef THEME_HPP
#define THEME_HPP
#include <SDL2/SDL_pixels.h>

typedef enum {
  PRIMARY = 0,
  SECONDARY = 1,
  TERTIARY = 2,
  TEXT = 3,
  TEXT_BG = 4,
  BG = 5
} STRING_ENUM_REF;

class ProgramThemes {
public:
  ProgramThemes();
  ~ProgramThemes();
  void set_color(SDL_Color to_color, int type);
  SDL_Color *get_primary();
  SDL_Color *get_secondary();
  SDL_Color *get_tertiary();
  SDL_Color *get_text();
  SDL_Color *get_textbg();

private:
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color tertiary;
  SDL_Color text;
  SDL_Color text_bg;
};

#endif
