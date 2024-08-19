#include "../include/events.hpp"

std::pair<int, int>
SDL2KeyInputs::poll_events() {
  SDL_Event e;

  std::pair<int, int> event_pair;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {

    case SDL_KEYDOWN: {
      event_pair.first  = KEYBOARD_PRESS;
      event_pair.second = e.key.keysym.sym;

      return event_pair;
    }

    case SDL_KEYUP: {
      event_pair.first  = KEYBOARD_RELEASE;
      event_pair.second = e.key.keysym.sym;

      return event_pair;
    }

    case SDL_QUIT: {
      event_pair.first  = QUIT;
      event_pair.second = 0;

      return event_pair;
    }
    default: {
      event_pair.first  = DEFAULT_CASE;
      event_pair.second = 0;

      return event_pair;
    }
    }
  }

  event_pair.first  = DEFAULT_CASE;
  event_pair.second = 0;

  return event_pair;
}
