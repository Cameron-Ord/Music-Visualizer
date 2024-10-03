#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "internal.hpp"
#include "fonts.hpp"
#include "rendering.hpp"
#include "window.hpp"
#include "events.hpp"
#include "audio.hpp"

extern SDL2INTERNAL sdl2;
extern SDL2Audio sdl2_ad;
extern SDL2Renderer rend;
extern SDL2Window win;
extern SDL2KeyInputs key;
extern SDL2Fonts fonts;

#endif