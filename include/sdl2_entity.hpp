#ifndef SDL2_ENTITY_HPP
#define SDL2_ENTITY_HPP
#include "structdef.h"
#include "render_entity.hpp"
#include "window_entity.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

typedef enum {
    AT_DIRECTORIES = 0,
    AT_SONGS = 1,
    LISTENING = 2,
} USER_STATE;


class SDL2INTERNAL {
  public:
    ~SDL2INTERNAL();
    SDL2INTERNAL();

    bool initialize_sdl2_video();
    bool initialize_sdl2_events();
    bool initialize_sdl2_audio();
    bool initialize_sdl2_ttf();
    bool initialize_sdl2_img();
    bool get_play_state();

    const WIN_SIZE *get_stored_window_size();
    const WIN_SIZE get_current_window_size(SDL_Window *window);

    int get_current_user_state();
    void set_current_user_state(int USER_STATE_ENUM_VALUE);

    void set_window_size(WIN_SIZE);
    void set_play_state(bool state);

  private:
    int user_state;
    WIN_SIZE window_size_values;
    bool play_state;
};
#endif
