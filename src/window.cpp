#include "../include/window_entity.hpp"

void *SDL2Window::create_window(SDL_Window **w) {
    *w = SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!*w) {
        fprintf(stderr, "Could not create window! -> %s\n", SDL_GetError());
        return NULL;
    }

    return *w;
}

SDL_Window **SDL2Window::get_window() {
    return &w;
}
SDL2Window::SDL2Window() {
    w = NULL;
}
