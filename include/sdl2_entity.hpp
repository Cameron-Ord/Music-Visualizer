#ifndef SDL2_ENTITY_HPP
#define SDL2_ENTITY_HPP
#include "events.hpp"
#include "files.hpp"
#include "font_entity.h"
#include "render_entity.h"
#include "theme.hpp"
#include "window_entity.h"
#include <SDL2/SDL.h>

typedef enum {
  WINDOW = 0,
  FONT = 1,
  RENDERER = 2,
  KEY_INPUT = 3,
  THEMES = 4,
  FILES = 5
} CLASS_ENUM_MAP;

class SDL2INTERNAL {
public:
  ~SDL2INTERNAL();
  SDL2INTERNAL();
  int initialize_sdl2_video();
  int initialize_sdl2_events();
  int initialize_sdl2_audio();
  int initialize_sdl2_ttf();
  int initialize_sdl2_image();

  void set_entity(void *void_class_ptr, int CLASS_ENUM_VALUE);
  void *get_entity(int CLASS_ENUM_VALUE);

  int get_play_state();
  void set_play_state(int bool_num);

private:
  int play_state;
  SDL2Window *_window_context;
  SDL2Fonts *_font_context;
  SDL2Renderer *_render_context;
  SDL2KeyInputs *_key_input_context;
  ProgramThemes *_themes;
  ProgramFiles *_files;
};
#endif
