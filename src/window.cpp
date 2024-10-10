#include "../include/window.hpp"
#include <cstdio>

void SDL2Window::set_border_bool(int value) { is_window_bordered = value; }

const int *SDL2Window::get_border_bool() { return &is_window_bordered; }

void SDL2Window::set_window(SDL_Window *ptr) { w = ptr; }
SDL_Window *SDL2Window::get_window() { return w; }
SDL2Window::SDL2Window() { w = NULL; }
