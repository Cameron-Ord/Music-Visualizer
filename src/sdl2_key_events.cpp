#include "../include/sdl2_entity.hpp"

int SDL2KeyInputs::poll_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT: {
      return QUIT;
    }
    default: {
      return DEFAULT_CASE;
    }
    }
  }
  return DEFAULT_CASE;
}
