#ifndef SDL2_ENTITY_HPP
#define SDL2_ENTITY_HPP

#include "sdl2defs.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int scc(int code);
void* scp(void* ptr);

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
  int no_mouse_grab;
  int user_state;
  WIN_SIZE window_size_values;
  bool play_state;
};

#endif
