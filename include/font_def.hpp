#ifndef FONT_DEF_HPP
#define FONT_DEF_HPP

#include <SDL2/SDL_render.h>
#include <string>

struct Text
{
    SDL_Texture *tex;
    SDL_Surface *surf;
    SDL_Rect rect;
    int width;
    int height;
    size_t id;
    bool is_valid;
    std::string name;
};

typedef Text Text;

#endif
