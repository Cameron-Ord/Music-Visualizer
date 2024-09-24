#ifndef FILE_DEF_HPP
#define FILE_DEF_HPP
#include <SDL2/SDL_render.h>
#include <string>

struct Icon {
    std::string file_name;
    SDL_Texture *texture;
    SDL_Surface *surface;
    int is_valid;
};

struct Directory {
    std::string directory_name;
    size_t directory_id;
};

struct Files {
    std::string file_name;
    size_t file_id;
};

typedef Icon Icon;
typedef Directory Directory;
typedef Files Files;

#endif
