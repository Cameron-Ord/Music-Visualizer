#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <string>
#include <vector>

#include "font_def.hpp"

typedef enum
{
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
} SDL_KEYCODES_ENUM;

class SDL2KeyInputs
{
  public:
    SDL2KeyInputs();

    void reset_cursor_index(size_t *cursor_index_ptr);
    std::pair<int, SDL_Keysym> poll_events();
    void cycle_up_list(size_t max_length, size_t *cursor_index_ptr);
    void cycle_down_list(size_t max_length, size_t *cursor_index_ptr);
    size_t *get_song_cursor_index();
    size_t *get_dir_cursor_index();
    void affirm_index();
    std::string select_element(const std::vector<Text> *d,
                               size_t *cursor_index_ptr);

  private:
    size_t cursor_index_dirs;
    size_t cursor_index_songs;
};

#endif
