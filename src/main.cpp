#include "../include/events.hpp"
#include "../include/files.hpp"
#include "../include/macdefs.hpp"
#include "../include/program_path.hpp"
#include "../include/render_entity.hpp"
#include "../include/sdl2_entity.hpp"
#include "../include/theme.hpp"
#include "../include/window_entity.hpp"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char* argv[]) {
  // instantiate classes
  bool          err;
  SDL2INTERNAL  sdl2;
  SDL2Renderer  rend;
  SDL2Window    win;
  SDL2KeyInputs key;
  ProgramThemes themes;
  ProgramPath   pathing;
  ProgramFiles  files;
  SDL2Fonts     fonts;

  if (!sdl2.initialize_sdl2_video()) {
    fprintf(stdout, "Failed to initialize SDL2 video!\n");
    return 1;
  }

  win.create_window(win.get_window());
  if (win.get_window() == NULL) {
    fprintf(stderr, "Failed to initialize SDL2 window! -> WINDOW POINTER WAS NULL\n");
    SDL_Quit();
    return 1;
  }

  std::pair<int, int> sizes = sdl2.get_current_window_size(*win.get_window());
  sdl2.set_window_size(sizes);

  rend.create_renderer(win.get_window(), rend.get_renderer());
  if (*rend.get_renderer() == NULL) {
    fprintf(stderr, "Failed to initialize SDL2 renderer -> RENDER POINTER WAS NULL!\n");
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_events()) {
    fprintf(stderr, "Failed to initialize SDL2 inputs! -> EXIT\n");
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_audio()) {
    fprintf(stderr, "Failed to initialize SDL2 audio! -> EXIT\n");
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_image()) {
    fprintf(stderr, "Failed to initialize SDL2 image! -> EXIT\n");
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_ttf()) {
    fprintf(stderr, "Failed to initialize SDL2 ttf! -> EXIT\n");
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  if (!fonts.open_font()) {
    fprintf(stderr, "Failed to open font! -> EXIT\n");
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  err = pathing.create_music_source();
  if (!err) {
    fprintf(stderr, "Failed to create or acknowledge existence of source "
                    "directories! -> EXIT\n");
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  err = pathing.create_log_directories();
  if (!err) {
    fprintf(stderr, "Failed to create or acknowledge existence of logging "
                    "directories! -> EXIT\n");
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  err = files.fill_directories(pathing.get_src_path(), pathing.return_slash());
  if (!err) {
    fprintf(stdout, "Error, or no directories found!\n");
  }

  fonts.create_dir_text(*files.retrieve_directories(), *rend.get_renderer(), *themes.get_text(),
                        fonts.get_font_ptr());

  uint64_t frame_start;
  int      frame_time;

  const std::string logging_src_path = pathing.get_logging_path();
  const std::string log_file_concat  = pathing.join_str(logging_src_path, "log.txt");

  FILE* std_out_file = freopen(log_file_concat.c_str(), "a", stdout);
  if (std_out_file == NULL) {
    fprintf(stderr, "Could not redirect STDOUT! -> %s\n", strerror(errno));
  }

  const std::string errlog_file_concat = pathing.join_str(logging_src_path, "errlog.txt");

  FILE* std_err_file = freopen(errlog_file_concat.c_str(), "a", stderr);
  if (std_err_file == NULL) {
    fprintf(stderr, "Could not redirect STDERR! -> %s\n", strerror(errno));
  }

  sdl2.set_entity(&win, WINDOW);
  sdl2.set_entity(&rend, RENDERER);
  sdl2.set_entity(&fonts, FONT);
  sdl2.set_entity(&key, KEY_INPUT);
  sdl2.set_entity(&themes, THEMES);
  sdl2.set_entity(&pathing, PATHS);
  sdl2.set_entity(&files, FILES);

  const int ticks_per_frame = (1000.0 / 60);
  sdl2.set_play_state(true);

  while (sdl2.get_play_state()) {
    rend.render_clear(*rend.get_renderer());
    rend.render_bg(*rend.get_renderer(), themes.get_secondary());

    rend.render_set_directories(sdl2.get_stored_window_size(), fonts.get_dir_vec());
    rend.render_draw_directories(*rend.get_renderer(), fonts.get_dir_vec());

    frame_start                            = SDL_GetTicks64();
    const std::pair<int, int> event_return = key.poll_events();

    const int event_type = event_return.first;
    const int keycode    = event_return.second;

    switch (event_type) {
    case KEYBOARD_PRESS: {
      switch (keycode) {
      case Q: {
        sdl2.set_play_state(false);
        break;
      }
      case UP: {
        break;
      }
      case DOWN: {
        break;
      }
      case LEFT: {
        break;
      }
      case RIGHT: {
        break;
      }
      case ENTER: {
        break;
      }
      case BACKSPACE: {
        break;
      }
      case SPACE: {
        break;
      }
      }
      break;
    }

    case KEYBOARD_RELEASE: {
      switch (keycode) {
      case Q: {
        sdl2.set_play_state(false);
        break;
      }
      case UP: {
        break;
      }
      case DOWN: {
        break;
      }
      case LEFT: {
        break;
      }
      case RIGHT: {
        break;
      }
      case ENTER: {
        break;
      }
      case BACKSPACE: {
        break;
      }
      case SPACE: {
        break;
      }
      }
      break;
    }

    case QUIT: {
      sdl2.set_play_state(false);
      break;
    }

    case DEFAULT_CASE: {
      break;
    }

    default: {
      break;
    }
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    rend.render_present(*rend.get_renderer());
  }

  if (std_out_file != NULL) {
    fclose(std_out_file);
  }

  if (std_err_file != NULL) {
    fclose(std_err_file);
  }

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
