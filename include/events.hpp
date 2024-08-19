#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <string>

typedef enum {
  Q         = SDLK_q,
  UP        = SDLK_UP,
  DOWN      = SDLK_DOWN,
  LEFT      = SDLK_LEFT,
  RIGHT     = SDLK_RIGHT,
  ENTER     = SDLK_RETURN,
  BACKSPACE = SDLK_BACKSPACE,
  SPACE     = SDLK_SPACE,
} SDL_KEYCODES_ENUM;

typedef enum {
  DEFAULT_CASE        = 0,
  QUIT                = 1,
  MOUSE_CLICKED       = 2,
  KEYBOARD_PRESS      = 3,
  KEYBOARD_RELEASE    = 4,
  WINDOW_SIZE_CHANGED = 5
} EVENT_RETURN;

class SDL2KeyInputs {
public:
  SDL2KeyInputs();

  std::pair<int, int> poll_events();
  void                cycle_up_list(size_t* cursor_index, size_t max_length);
  void                cycle_down_list(size_t* cursor_index, size_t max_length);
  size_t*             get_cursor_index();

private:
  size_t cursor_index;
};

#endif
