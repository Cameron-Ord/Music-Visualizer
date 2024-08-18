#ifndef EVENTS_HPP
#define EVENTS_HPP
#include <SDL2/SDL.h>

typedef enum { Q = SDL_SCANCODE_Q, A = SDL_SCANCODE_A } SDL_SCANCODES_ENUM;
typedef enum {
  DEFAULT_CASE = 0,
  QUIT = 1,
  MOUSE_CLICKED = 2,
  KEYBOARD_PRESS = 3,
  KEYBOARD_RELEASE = 4
} EVENT_RETURN;

class SDL2KeyInputs {
public:
  int poll_events();

private:
};

#endif
