#include "../include/theme.hpp"

ProgramThemes::ProgramThemes() {
    SDL_Color primary = { 166, 172, 205, 255 };
    SDL_Color secondary = { 68, 66, 103, 255 };
    SDL_Color background = { 41, 45, 62, 255 };
    SDL_Color text = { 103, 110, 149, 255 };
    SDL_Color text_bg = { 113, 124, 180, 255 };

    set_color(primary, PRIMARY);
    set_color(secondary, SECONDARY);
    set_color(background, BACKGROUND);
    set_color(text, TEXT);
    set_color(text_bg, TEXT_BG);
}

ProgramThemes::~ProgramThemes() {}

SDL_Color *ProgramThemes::get_primary() {
    return &primary;
}

SDL_Color *ProgramThemes::get_secondary() {
    return &secondary;
}

SDL_Color *ProgramThemes::get_background() {
    return &background;
}

SDL_Color *ProgramThemes::get_text() {
    return &text;
}

SDL_Color *ProgramThemes::get_textbg() {
    return &text_bg;
}

void ProgramThemes::set_color(SDL_Color to_color, int TYPE) {
    switch (TYPE) {
    case PRIMARY: {
        primary = to_color;
        break;
    }
    case SECONDARY: {
        secondary = to_color;
        break;
    }
    case BACKGROUND: {
        background = to_color;
        break;
    }
    case TEXT: {
        text = to_color;
        break;
    }
    case TEXT_BG: {
        text_bg = to_color;
        break;
    }
    default: {
        break;
    }
    }
}
