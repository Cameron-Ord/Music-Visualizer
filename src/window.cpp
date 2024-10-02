#include "../include/window.hpp"
#include <cstdio>

void *SDL2Window::create_window(SDL_Window **w) {
    *w = SDL_CreateWindow(
        "Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600,
        400, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!*w) {
        fprintf(stderr, "Could not create window! -> %s\n", SDL_GetError());
        return NULL;
    }
    
    is_window_bordered = 0;
    return *w;
}

void SDL2Window::set_border_bool(int value) {
    is_window_bordered = value;
}

const int *SDL2Window::get_border_bool() {
    return &is_window_bordered;
}

SDL_Window **SDL2Window::get_window() {
    return &w;
}
SDL2Window::SDL2Window() {
    w = NULL;
}
