#include "../include/theme.hpp"

ProgramThemes::ProgramThemes() {
    SDL_Color primary_nord = {129, 161, 193, 255};
    SDL_Color secondary_nord = {46, 52, 64, 255};
    SDL_Color tertiary_nord = {136, 192, 208, 255};
    SDL_Color text_nord = {216, 222, 233, 255};
    SDL_Color text_bg_nord = {94, 129, 172, 255};

    set_color(primary_nord, PRIMARY);
    set_color(secondary_nord, SECONDARY);
    set_color(tertiary_nord, TERTIARY);
    set_color(text_nord, TEXT);
    set_color(text_bg_nord, TEXT_BG);
}

ProgramThemes::~ProgramThemes() {}

SDL_Color* ProgramThemes::get_primary() {
    return &primary;
}

SDL_Color* ProgramThemes::get_secondary() {
    return &secondary;
}

SDL_Color* ProgramThemes::get_tertiary() {
    return &tertiary;
}

SDL_Color* ProgramThemes::get_text() {
    return &text;
}

SDL_Color* ProgramThemes::get_textbg() {
    return &text_bg;
}

void ProgramThemes::set_color(SDL_Color to_color, int TYPE) {
    switch (TYPE) {
        case PRIMARY : {
            primary = to_color;
            break;
        }
        case SECONDARY : {
            secondary = to_color;
            break;
        }
        case TERTIARY : {
            tertiary = to_color;
            break;
        }
        case TEXT : {
            text = to_color;
            break;
        }
        case TEXT_BG : {
            text_bg = to_color;
            break;
        }
        default : {
            break;
        }
    }
}
