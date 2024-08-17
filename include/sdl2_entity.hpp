#ifndef SDL2_ENTITY_H
#define SDL2_ENTITY_H
#include "events.hpp"
#include "render_entity.h"
#include "window_entity.h"
#include <SDL2/SDL.h>

class SDL2Renderer;
class SDL2Window;
class SDL2KeyInputs;
class ProgramPath;
class SDL2INTERNAL;

class SDL2INTERNAL {
public:
  ~SDL2INTERNAL();
  SDL2INTERNAL();
  int initialize_sdl2_video();
  int initialize_sdl2_events();
  int initialize_sdl2_audio();
  SDL2Window *get_window_entity();
  SDL2Renderer *get_render_entity();
  SDL2KeyInputs *get_key_input_entity();
  int get_play_state();
  void set_play_state(int bool_num);

private:
  int play_state;
  SDL2Window *_window_context;
  SDL2Renderer *_render_context;
  SDL2KeyInputs *_key_input_context;
};
#endif
