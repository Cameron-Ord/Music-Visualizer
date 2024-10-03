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

  private:
    SDL_Color primary;
    SDL_Color secondary;
    SDL_Color background;
    SDL_Color text;
    SDL_Color text_bg;
};

#endif
