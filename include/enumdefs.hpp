#ifndef ENUMDEFS_HPP
#define ENUMDEFS_HPP

#include "SDL2/SDL_events.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>

typedef enum {
    MOUSEBTN_DOWN = SDL_MOUSEBUTTONDOWN,
    MOUSEBTN_UP = SDL_MOUSEBUTTONUP,
    MOUSE_LEFT = SDL_BUTTON_LEFT,
    MOUSE_MOVE = SDL_MOUSEMOTION

} MOUSE_MAPPINGS;

typedef enum {
    FLOATS = 0,
    INTS = 1,
} SETTING_RENDER_MODE;

typedef enum {
    AT_DIRECTORIES = 0,
    AT_SONGS = 1,
    LISTENING = 2,
    AT_SETTINGS = 3,
} USER_STATE;



typedef enum {
    Q = SDLK_q,
    UP = SDLK_UP,
    DOWN = SDLK_DOWN,
    LEFT = SDLK_LEFT,
    RIGHT = SDLK_RIGHT,
    ENTER = SDLK_RETURN,
    BACKSPACE = SDLK_BACKSPACE,
    SPACE = SDLK_SPACE,
    ESCAPE = SDLK_ESCAPE,
    P_KEY = SDLK_p,
    L = SDLK_l,
    // up
    K = SDLK_k,
    H = SDLK_h,
    // down
    J = SDLK_j,
    T = SDLK_t

} SDL_KEYCODES_ENUM;



typedef enum { STOPPED = 0, PLAYING = 1, PAUSED = 2 } STREAM_FLAGS;
typedef enum { WAITING = 0, NEXT = 1 } PB_FLAGS;

#endif