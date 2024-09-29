#include "../include/sdl2_entity.hpp"

SDL2INTERNAL::SDL2INTERNAL() {
    play_state = 1;
}

SDL2INTERNAL::~SDL2INTERNAL() {}

int SDL2INTERNAL::get_current_user_state() {
    return user_state;
}

void SDL2INTERNAL::set_current_user_state(int USER_STATE_ENUM_VALUE) {
    user_state = USER_STATE_ENUM_VALUE;
}

bool SDL2INTERNAL::initialize_sdl2_img() {
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "Could not initialize SDL2 image ->" << SDL_GetError()
                  << std::endl;
        return false;
    }
    return true;
}

bool SDL2INTERNAL::initialize_sdl2_events() {
    if (SDL_Init(SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "SDL2 Could not initialize events -> %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

bool SDL2INTERNAL::initialize_sdl2_video() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL2 Could not initalize video -> %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

bool SDL2INTERNAL::initialize_sdl2_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL2 could not initialize audio -> %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

bool SDL2INTERNAL::initialize_sdl2_ttf() {
    if (TTF_Init() != 0) {
        fprintf(stderr, "SDL2 TTF could not initialize -> %s\n",
                SDL_GetError());
        return false;
    }
    return true;
}

const WIN_SIZE SDL2INTERNAL::get_current_window_size(SDL_Window *window) {
    int w, h;
    WIN_SIZE sizes;

    SDL_GetWindowSize(window, &w, &h);

    sizes.WIDTH = w;
    sizes.HEIGHT = h;

    return sizes;
}

const WIN_SIZE *SDL2INTERNAL::get_stored_window_size() {
    return &window_size_values;
}

void SDL2INTERNAL::set_window_size(WIN_SIZE sizes) {
    window_size_values.WIDTH = sizes.WIDTH;
    window_size_values.HEIGHT = sizes.HEIGHT;
}

void SDL2INTERNAL::set_play_state(bool state) {
    play_state = state;
}

bool SDL2INTERNAL::get_play_state() {
    return play_state;
}
