#ifndef SWITCH_HPP
#define SWITCH_HPP
#include "structdef.h"
#include <cstdint>
void handle_window_event(std::uint8_t event, StdClassWrapper *std, SDL2Wrapper* sdl2_w);
void handle_keydown();
void handle_keyup();

#endif