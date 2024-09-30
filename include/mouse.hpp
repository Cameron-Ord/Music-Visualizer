#ifndef MOUSE_HPP
#define MOUSE_HPP
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <stdbool.h>

typedef enum {
    MOUSEBTN_DOWN = SDL_MOUSEBUTTONDOWN,
    MOUSEBTN_UP = SDL_MOUSEBUTTONUP,
    MOUSE_LEFT = SDL_BUTTON_LEFT,
    MOUSE_MOVE = SDL_MOUSEMOTION

} MOUSE_MAPPINGS;

struct Mouse {
    int mouse_x;
    int mouse_y;
    bool held;
};

#endif