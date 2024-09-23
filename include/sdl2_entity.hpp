#ifndef SDL2_ENTITY_HPP
#define SDL2_ENTITY_HPP

class SDL2Fonts;
class SDL2Renderer;
class SDL2KeyInputs;
class ProgramThemes;
class ProgramPath;
class ProgramFiles;
class SDL2Window;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

typedef enum {
  WINDOW = 0,
  FONT = 1,
  RENDERER = 2,
  KEY_INPUT = 3,
  THEMES = 4,
  FILES = 5,
  PATHS = 6
} CLASS_ENUM_MAP;

typedef enum {
  AT_DIRECTORIES = 0,
  AT_SONGS = 1,
  LISTENING = 2,
} USER_STATE;

struct WIN_SIZE {
  int WIDTH;
  int HEIGHT;
};

typedef WIN_SIZE WIN_SIZE;

class SDL2INTERNAL {
public:
  ~SDL2INTERNAL();
  SDL2INTERNAL();

  bool initialize_sdl2_video();
  bool initialize_sdl2_events();
  bool initialize_sdl2_audio();
  bool initialize_sdl2_ttf();
  bool get_play_state();

  const WIN_SIZE *get_stored_window_size();
  const WIN_SIZE get_current_window_size(SDL_Window *window);

  int get_current_user_state();
  void set_current_user_state(int USER_STATE_ENUM_VALUE);

  void set_window_size(WIN_SIZE);
  void set_play_state(bool state);

private:
  int user_state;
  WIN_SIZE window_size_values;
  bool play_state;
};
#endif
