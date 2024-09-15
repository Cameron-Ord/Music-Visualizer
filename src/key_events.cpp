#include "../include/events.hpp"
#include <algorithm>
#include <list>

SDL2KeyInputs::SDL2KeyInputs() { cursor_index = 0; }

std::pair<int, SDL_Keysym> SDL2KeyInputs::poll_events() {
  SDL_Event e;

  std::pair<int, SDL_Keysym> event_pair;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {

    case SDL_WINDOWEVENT: {
      switch (e.window.event) {

      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        event_pair.first = WINDOW_SIZE_CHANGED;
        event_pair.second = e.key.keysym;

        return event_pair;
      }

      case SDL_WINDOWEVENT_RESIZED: {
        event_pair.first = WINDOW_SIZE_CHANGED;
        event_pair.second = e.key.keysym;

        return event_pair;
      }
      default: {
        break;
      }
      }
      break;
    }

    case SDL_KEYDOWN: {
      event_pair.first = KEYBOARD_PRESS;
      event_pair.second = e.key.keysym;

      return event_pair;
    }

    case SDL_KEYUP: {
      event_pair.first = KEYBOARD_RELEASE;
      event_pair.second = e.key.keysym;

      return event_pair;
    }

    case SDL_QUIT: {
      event_pair.first = QUIT;
      event_pair.second = e.key.keysym;

      return event_pair;
    }
    default: {
      break;
    }
    }
  }

  event_pair.first = DEFAULT_CASE;
  event_pair.second = e.key.keysym;

  return event_pair;
}

void SDL2KeyInputs::reset_cursor_index() { cursor_index = 0; }

void SDL2KeyInputs::cycle_up_list(size_t *cursor_index, size_t max_length) {
  int tmp = *cursor_index;

  tmp -= 1;
  if (tmp < 0) {
    tmp = (int)max_length - 1;
  }

  *cursor_index = (size_t)tmp;
}

void SDL2KeyInputs::cycle_down_list(size_t *cursor_index, size_t max_length) {
  size_t tmp = *cursor_index;
  tmp += 1;

  if (tmp > max_length - 1) {
    tmp = 0;
  }

  *cursor_index = tmp;
}

size_t *SDL2KeyInputs::get_cursor_index() { return &cursor_index; }

std::string SDL2KeyInputs::select_element(const size_t cursor_index,
                                          const size_t list_index,
                                          const std::vector<Text> *d) {
  return (*d)[cursor_index + list_index].name;
}
