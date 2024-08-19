#ifndef SDL2_ENTITY_HPP
#define SDL2_ENTITY_HPP
#include <string>
#include <utility>

class SDL2Fonts;
class SDL2Renderer;
class SDL2KeyInputs;
class ProgramThemes;
class ProgramPath;
class ProgramFiles;
class SDL2Window;

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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

class SDL2INTERNAL {
public:
  ~SDL2INTERNAL();
  SDL2INTERNAL();

  bool initialize_sdl2_video();
  bool initialize_sdl2_events();
  bool initialize_sdl2_audio();
  bool initialize_sdl2_ttf();
  bool initialize_sdl2_image();

  void set_entity(void *void_class_ptr, int CLASS_ENUM_VALUE);
  void *get_entity(int CLASS_ENUM_VALUE);

  bool get_play_state();

  std::pair<int, int> get_stored_window_size();
  std::pair<int, int> get_current_window_size(SDL_Window *window);
  int get_current_user_state();
  void set_current_user_state(int USER_STATE_ENUM_VALUE);

  void set_window_size(std::pair<int, int> size);
  void set_play_state(bool state);

private:
  int user_state;
  int win_width;
  int win_height;
  bool play_state;
  SDL2Window *_window_context;
  SDL2Fonts *_font_context;
  SDL2Renderer *_render_context;
  SDL2KeyInputs *_key_input_context;
  ProgramThemes *_themes;
  ProgramPath *_path;
  ProgramFiles *_files;
};
#endif
