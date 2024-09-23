#include "../include/events.hpp"


SDL2KeyInputs::SDL2KeyInputs() { cursor_index = 0; }

void SDL2KeyInputs::reset_cursor_index() { cursor_index = 0; }

const size_t *SDL2KeyInputs::get_cursor_index() { return &cursor_index; }

void SDL2KeyInputs::cycle_up_list(size_t max_length) {
  int tmp = cursor_index;

  tmp -= 1;
  if (tmp < 0) {
    tmp = (int)max_length - 1;
  }

  cursor_index = (size_t)tmp;
}

void SDL2KeyInputs::cycle_down_list(size_t max_length) {
  size_t tmp = cursor_index;
  tmp += 1;

  if (tmp > max_length - 1) {
    tmp = 0;
  }

  cursor_index = tmp;
}

std::string SDL2KeyInputs::select_element(const std::vector<Text> *d) {
  return (*d)[cursor_index].name;
}
