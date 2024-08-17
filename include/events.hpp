#ifndef EVENTS_H
#define EVENTS_H
#include <SDL2/SDL.h>

typedef enum { Q = SDL_SCANCODE_Q, A = SDL_SCANCODE_A } SDL_SCANCODES_ENUM;
typedef enum { DEFAULT_CASE = 0, QUIT_GAME = 1 } EVENT_RETURN;

class SDL2KeyInputs {
public:
  int poll_events();

private:
};

#endif
