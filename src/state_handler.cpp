#include "../include/globals.hpp"
#include "../include/switch.hpp"
#include "../include/theme.hpp"
#include <cstdint>

// Not gonna max limit cause thats LAME
const int MIN_WIDTH = 200;
const int MIN_HEIGHT = 100;

void keydown_handle_state(int userstate, SDL_Keysym sym, ProgramPath *pathing,
                          ProgramFiles *files, AudioData *ad,
                          FourierTransform *fft, ProgramThemes *themes) {
  switch (userstate) {
  default: {
    break;
  }

  case AT_DIRECTORIES: {
    directory_keydown_options(sym.sym, pathing, themes, files);
    break;
  }

  case AT_SONGS: {
    song_keydown_options(sym.sym, files, ad, pathing);
    break;
  }

  case LISTENING: {
    playback_keydown_options(sym.sym);
    break;
  }

  case AT_SETTINGS: {
    settings_keydown_options(sym.sym, sym.mod, fft);
  }
  }
}

void settings_keydown_options(SDL_Keycode sym, uint16_t mod,
                              FourierTransform *fft) {

  switch (sym) {
  default: {
    break;
  }

  case L: {
    increase_win_width(win.get_window());
    break;
  }

  case K: {
    increase_win_height(win.get_window());
    break;
  }

  case H: {
    decrease_win_width(win.get_window());
    break;
  }

  case J: {
    decrease_win_height(win.get_window());
    break;
  }

  case P_KEY: {
    if (*sdl2_ad.get_stream_flag() == PLAYING) {
      std::cout << "Paused" << std::endl;
      sdl2_ad.pause_audio();
      sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
    } else if (*sdl2_ad.get_stream_flag() == PAUSED) {
      std::cout << "Playing" << std::endl;
      sdl2_ad.resume_audio();
      sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
    }
    break;
  }

  case UP: {

    switch (*rend.get_setting_render_mode()) {
    default: {
      break;
    }

    case INTS: {
      if (SDL_Keymod(mod & KMOD_SHIFT)) {
        std::vector<SettingTextInt> *i = fonts.get_int_settings_vec();
        int mutable_value = *(*i)[*key.get_settings_cursor()].setting_value_ptr;
        mutable_value += 1;

        if (mutable_value > INT_SETTING_MAX) {
          mutable_value = INT_SETTING_MAX;
        }

        std::string setting_name =
            (*i)[*key.get_settings_cursor()].setting_text.name;

        if (setting_name == "Smoothing") {
          fft->set_smoothing(mutable_value);
        } else if (setting_name == "Smears") {
          fft->set_smear(mutable_value);
        }

      } else {
        int signed_index = static_cast<int>(*key.get_settings_cursor());

        signed_index -= 1;
        if (signed_index < 0) {
          signed_index = 0;
        }

        key.set_settings_cursor(static_cast<size_t>(signed_index));
      }
      break;
    }

    case FLOATS: {
      if (SDL_Keymod(mod & KMOD_SHIFT)) {
        std::vector<SettingTextFloat> *f = fonts.get_float_settings_vec();
        float mutable_value =
            *(*f)[*key.get_settings_cursor()].setting_value_ptr;
        mutable_value += 0.025;

        if (mutable_value > FLOAT_SETTING_MAX) {
          mutable_value = FLOAT_SETTING_MAX;
        }

        fft->set_filter_coeff(*key.get_settings_cursor(), mutable_value);

      } else {
        int signed_index = static_cast<int>(*key.get_settings_cursor());

        signed_index -= 1;
        if (signed_index < 0) {
          signed_index = 0;
        }

        key.set_settings_cursor(static_cast<size_t>(signed_index));
      }
      break;
    }
    }
    break;
  }

  case DOWN: {
    switch (*rend.get_setting_render_mode()) {
    default: {
      break;
    }

    case INTS: {
      if (SDL_Keymod(mod & KMOD_SHIFT)) {
        std::vector<SettingTextInt> *i = fonts.get_int_settings_vec();
        int mutable_value = *(*i)[*key.get_settings_cursor()].setting_value_ptr;
        mutable_value -= 1;

        if (mutable_value < INT_SETTING_MIN) {
          mutable_value = INT_SETTING_MIN;
        }

        std::string setting_name =
            (*i)[*key.get_settings_cursor()].setting_text.name;

        if (setting_name == "Smoothing") {
          fft->set_smoothing(mutable_value);
        } else if (setting_name == "Smears") {
          fft->set_smear(mutable_value);
        }
      } else {

        int size = static_cast<int>(fonts.get_int_settings_vec()->size());
        int signed_index = static_cast<int>(*key.get_settings_cursor());

        signed_index += 1;
        if (signed_index > size - 1) {
          signed_index = size - 1;
        }

        key.set_settings_cursor(static_cast<size_t>(signed_index));
      }
      break;
    }

    case FLOATS: {
      if (SDL_Keymod(mod & KMOD_SHIFT)) {
        std::vector<SettingTextFloat> *f = fonts.get_float_settings_vec();
        float mutable_value =
            *(*f)[*key.get_settings_cursor()].setting_value_ptr;
        mutable_value -= 0.025;

        if (mutable_value < FLOAT_SETTING_MIN) {
          mutable_value = FLOAT_SETTING_MIN;
        }

        fft->set_filter_coeff(*key.get_settings_cursor(), mutable_value);
      } else {

        int size = static_cast<int>(fonts.get_float_settings_vec()->size());
        int signed_index = static_cast<int>(*key.get_settings_cursor());

        signed_index += 1;
        if (signed_index > size - 1) {
          signed_index = size - 1;
        }

        key.set_settings_cursor(static_cast<size_t>(signed_index));
      }
      break;
    }
    }
    break;
  }

  case LEFT: {
    if (SDL_Keymod(mod & KMOD_SHIFT)) {
      switch (*rend.get_setting_render_mode()) {
      default: {
        break;
      }
      case INTS: {
        rend.set_setting_render_mode(FLOATS);
        key.set_settings_cursor(0);
        break;
      }

      case FLOATS: {
        rend.set_setting_render_mode(INTS);
        key.set_settings_cursor(0);
        break;
      }
      }
    } else {
      if (*sdl2_ad.get_stream_flag() == PLAYING ||
          *sdl2_ad.get_stream_flag() == PAUSED) {
        sdl2.set_current_user_state(LISTENING);
      }
    }
    break;
  }

  case RIGHT: {
    if (SDL_Keymod(mod & KMOD_SHIFT)) {
      switch (*rend.get_setting_render_mode()) {
      default: {
        break;
      }

      case INTS: {
        rend.set_setting_render_mode(FLOATS);
        key.set_settings_cursor(0);
        break;
      }

      case FLOATS: {
        rend.set_setting_render_mode(INTS);
        key.set_settings_cursor(0);
        break;
      }
      }
    } else {
      sdl2.set_current_user_state(AT_DIRECTORIES);
    }
    break;
  }
  }
}

void select_directory(ProgramThemes *themes, ProgramFiles *files,
                      ProgramPath *pathing) {

  const size_t *virt_dir_vec_index = key.get_vdir_index();
  const size_t *virt_dir_cursor = key.get_vdir_cursor_index();
  const size_t directories_size = files->retrieve_directories()->size();

  std::string dirname;
  dirname = key.select_element(NULL, virt_dir_cursor);
  pathing->set_opened_dir(dirname);

  bool result = false;
  std::string concat_path = pathing->join_str(pathing->get_src_path(), dirname);

  // result = files->fill_files(concat_path, pathing->return_slash());
}

void select_song(ProgramFiles *files, ProgramPath *pathing, AudioData *ad) {

  sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
  sdl2_ad.pause_audio();
  sdl2_ad.close_audio_device();

  const size_t *virt_song_vec_index = key.get_vsong_index();
  const size_t *virt_song_cursor = key.get_vsong_cursor_index();
  const size_t files_size = files->retrieve_directory_files()->size();

  std::string filename;
  filename = key.select_element(NULL, virt_song_cursor);
  bool result = false;
  std::string concat_path =
      pathing->join_str(pathing->get_src_path(), pathing->get_opened_dir());
  result = ad->read_audio_file(pathing->join_str(concat_path, filename));

  if (result) {

    key.set_song_index(*virt_song_vec_index);
    key.set_song_cursor_index(*virt_song_cursor);

    sdl2_ad.set_audio_spec(ad->get_audio_data());
    sdl2_ad.open_audio_device();
    sdl2_ad.resume_audio();
    sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
    sdl2.set_current_user_state(LISTENING);
  }
}

void directory_keydown_options(SDL_Keycode sym, ProgramPath *pathing,
                               ProgramThemes *themes, ProgramFiles *files) {

  switch (sym) {
  default: {
    break;
  }

  case L: {
    increase_win_width(win.get_window());
    break;
  }

  case K: {
    increase_win_height(win.get_window());
    break;
  }

  case H: {
    decrease_win_width(win.get_window());
    break;
  }

  case J: {
    decrease_win_height(win.get_window());
    break;
  }

  case P_KEY: {
    if (*sdl2_ad.get_stream_flag() == PLAYING) {
      std::cout << "Paused" << std::endl;
      sdl2_ad.pause_audio();
      sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
    } else if (*sdl2_ad.get_stream_flag() == PAUSED) {
      std::cout << "Playing" << std::endl;
      sdl2_ad.resume_audio();
      sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
    }
    break;
  }

  case DOWN: {
    std::string result;
    size_t current_vec_size;
    const size_t *virtual_dir_cursor = key.get_vdir_cursor_index();
    const size_t *virtual_dvec_index = key.get_vdir_index();

  } break;

  case UP: {
    std::string result;
    size_t current_vec_size;
    const size_t *virtual_dir_cursor = key.get_vdir_cursor_index();
    const size_t *virtual_dvec_index = key.get_vdir_index();
  } break;

  case LEFT: {
    sdl2.set_current_user_state(AT_SETTINGS);
  } break;

  case RIGHT: {
    // sdl2.set_current_user_state(AT_SONGS);
    break;
  }

  case SPACE: {
    select_directory(themes, files, pathing);
    break;
  }
  }
}

void song_keydown_options(SDL_Keycode sym, ProgramFiles *files, AudioData *ad,
                          ProgramPath *pathing) {

  switch (sym) {
  default: {
    break;
  }

  case L: {
    increase_win_width(win.get_window());
    break;
  }

  case K: {
    increase_win_height(win.get_window());
    break;
  }

  case H: {
    decrease_win_width(win.get_window());
    break;
  }

  case J: {
    decrease_win_height(win.get_window());
    break;
  }

  case P_KEY: {
    if (*sdl2_ad.get_stream_flag() == PLAYING) {
      std::cout << "Paused" << std::endl;
      sdl2_ad.pause_audio();
      sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
    } else if (*sdl2_ad.get_stream_flag() == PAUSED) {
      std::cout << "Playing" << std::endl;
      sdl2_ad.resume_audio();
      sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
    }
    break;
  }

  case DOWN: {
    std::string result;
    size_t current_vec_size;
    const size_t *virtual_song_cursor = key.get_vsong_cursor_index();
    const size_t *virtual_svec_index = key.get_vsong_index();

  } break;

  case UP: {
    std::string result;
    size_t current_vec_size;
    const size_t *virtual_song_cursor = key.get_vsong_cursor_index();
    const size_t *virtual_svec_index = key.get_vsong_index();

  } break;

  case LEFT: {
    sdl2.set_current_user_state(AT_DIRECTORIES);
    break;
  }

  case RIGHT: {
    if (*sdl2_ad.get_stream_flag() == PLAYING ||
        *sdl2_ad.get_stream_flag() == PAUSED) {
      sdl2.set_current_user_state(LISTENING);
    }
    break;
  }

  case SPACE: {
    select_song(files, pathing, ad);
    break;
  }
  }
}

void playback_keydown_options(SDL_Keycode sym) {

  switch (sym) {
  default: {
    break;
  }

  case L: {
    increase_win_width(win.get_window());
    break;
  }

  case K: {
    increase_win_height(win.get_window());
    break;
  }

  case H: {
    decrease_win_width(win.get_window());
    break;
  }

  case J: {
    decrease_win_height(win.get_window());
    break;
  }

  case P_KEY: {
    if (*sdl2_ad.get_stream_flag() == PLAYING) {
      std::cout << "Paused" << std::endl;
      sdl2_ad.pause_audio();
      sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
    } else if (*sdl2_ad.get_stream_flag() == PAUSED) {
      std::cout << "Playing" << std::endl;
      sdl2_ad.resume_audio();
      sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
    }
    break;
  }

  case LEFT: {
    sdl2.set_current_user_state(AT_SONGS);

    break;
  }

  case RIGHT: {
    sdl2.set_current_user_state(AT_SETTINGS);

    break;
  }
  }
}

void handle_window_event(uint8_t event, ProgramFiles *files,
                         ProgramThemes *themes) {

  switch (event) {
  default: {
    break;
  }
  case SDL_WINDOWEVENT_RESIZED: {
    const size_t *song_cursor_before = key.get_vsong_cursor_index();
    const size_t *dir_cursor_before = key.get_vdir_cursor_index();

    const size_t *songs_index = key.get_vsong_index();
    const size_t *dirs_index = key.get_vdir_index();

    std::vector<Text> *text_vector = nullptr;

    std::string dir_name;
    std::string song_name;

    WIN_SIZE ws = sdl2.get_current_window_size(win.get_window());
    sdl2.set_window_size(ws);
    rend.set_font_draw_limit(ws.HEIGHT);
    fonts.set_char_limit(ws.WIDTH);

  } break;

  case SDL_WINDOWEVENT_SIZE_CHANGED: {
    const size_t *song_cursor_before = key.get_vsong_cursor_index();
    const size_t *dir_cursor_before = key.get_vdir_cursor_index();

    const size_t *songs_index = key.get_vsong_index();
    const size_t *dirs_index = key.get_vdir_index();

    std::vector<Text> *text_vector = nullptr;

    std::string dir_name;
    std::string song_name;

  } break;
  }
}

void decrease_win_width(SDL_Window *w) {
  int current_w, current_h;

  SDL_GetWindowSize(w, &current_w, &current_h);
  if (current_w - 10 <= MIN_WIDTH) {
    current_w = MIN_WIDTH;
  }
  SDL_SetWindowSize(w, current_w - 10, current_h);
}

void increase_win_width(SDL_Window *w) {
  int current_w, current_h;
  SDL_GetWindowSize(w, &current_w, &current_h);
  SDL_SetWindowSize(w, current_w + 10, current_h);
}

void increase_win_height(SDL_Window *w) {
  int current_w, current_h;
  SDL_GetWindowSize(w, &current_w, &current_h);

  SDL_SetWindowSize(w, current_w, current_h + 10);
}

void decrease_win_height(SDL_Window *w) {
  int current_w, current_h;
  SDL_GetWindowSize(w, &current_w, &current_h);
  if (current_h - 10 <= MIN_HEIGHT) {
    current_h = MIN_HEIGHT;
  }
  SDL_SetWindowSize(w, current_w, current_h - 10);
}
