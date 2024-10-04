#ifndef EVENTS_HPP
#define EVENTS_HPP

#include "audio.hpp"
#include "enumdefs.hpp"
#include "fft.hpp"
#include "files.hpp"
#include "fontdefs.hpp"
#include "paths.hpp"
#include "theme.hpp"
#include <SDL2/SDL_keyboard.h>
#include <iostream>
#include <vector>

class SDL2KeyInputs {
public:
  SDL2KeyInputs();

  void reset_cursor_index(size_t *cursor_index_ptr);
  std::pair<int, SDL_Keysym> poll_events();
  std::string check_cursor_move(size_t vec_size, const size_t *cursor_ptr,
                                std::string direction);
  void affirm_index();
  std::string select_element(const std::vector<Text> *d,
                             const size_t *cursor_index_ptr);
  const size_t *get_dir_index();
  const size_t *get_song_index();
  const size_t *get_vsong_index();
  const size_t *get_vdir_index();
  void set_dir_index(size_t i);
  void set_song_index(size_t i);
  void set_vdir_index(size_t i);
  void set_vsong_index(size_t i);

  const size_t *get_song_cursor_index();
  const size_t *get_vsong_cursor_index();
  const size_t *get_dir_cursor_index();
  const size_t *get_vdir_cursor_index();
  void set_song_cursor_index(size_t i);
  void set_dir_cursor_index(size_t i);
  void set_vsong_cursor_index(size_t i);
  void set_vdir_cursor_index(size_t i);

  void set_settings_cursor(size_t i);
  const size_t *get_settings_cursor();
  void input_handler(ProgramPath *pathing, ProgramFiles *files, AudioData *ad,
                     FourierTransform *fft, ProgramThemes *themes);
  void set_mouse_grab(int status);

private:
  int no_mouse_grab;
  size_t virtual_dvec_index;
  size_t virtual_svec_index;

  size_t virtual_cursor_dindex;
  size_t virtual_cursor_sindex;

  size_t directories_vec_index;
  size_t songs_vec_index;

  size_t cursor_index_dirs;
  size_t cursor_index_songs;

  size_t settings_cursor;
};

#endif
