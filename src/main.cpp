#include "../include/macdefs.hpp"
#include "../include/program_path.hpp"
#include "../include/sdl2_entity.hpp"
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // instantiate classes
  SDL2INTERNAL sdl2;
  SDL2Renderer rend;
  SDL2Window win;
  SDL2KeyInputs key;

  if (!sdl2.initialize_sdl2_video()) {
    fprintf(stdout, "Failed to initialize SDL2 video!\n");
    return 1;
  }

  // These pointers are null before they are given refs in the next few lines.
  SDL2Window *_window = sdl2.get_window_entity();
  SDL2Renderer *_renderer = sdl2.get_render_entity();
  SDL2KeyInputs *_key_inputs = sdl2.get_key_input_entity();

  _window = &win;
  _renderer = &rend;
  _key_inputs = &key;

  _window->create_window(_window->get_window());
  if (*_window->get_window() == NULL) {
    fprintf(stderr,
            "Failed to initialize SDL2 window! -> WINDOW POINTER WAS NULL\n");
    return 1;
  }

  _renderer->create_renderer(_window->get_window(), _renderer->get_renderer());
  if (*_renderer->get_renderer() == NULL) {
    fprintf(stderr,
            "Failed to initialize SDL2 renderer -> RENDER POINTER WAS NULL!\n");
    return 1;
  }

  if (!sdl2.initialize_sdl2_events()) {
    fprintf(stderr, "Failed to initialize SDL2 inputs!\n");
    return 1;
  }

  if (!sdl2.initialize_sdl2_audio()) {
    fprintf(stderr, "Failed to initialize SDL2 audio!\n");
    return 1;
  }

  ProgramPath pathing;
  int err;

  err = pathing.create_music_source();
  if (!err) {
    return 1;
  }

  err = pathing.create_log_directories();
  if (!err) {
    return 1;
  }

  uint64_t frame_start;
  int frame_time;

  SDL_Color rgba = {.r = 255, .g = 255, .b = 255, .a = 255};

  const std::string logging_src_path = pathing.get_logging_path();
  const std::string log_file_concat =
      pathing.join_str(logging_src_path, "log.txt");

  FILE *std_out_file = freopen(log_file_concat.c_str(), "a", stdout);
  if (std_out_file == NULL) {
    fprintf(stderr, "Could not redirect STDOUT! -> %s\n", strerror(errno));
  }

  const std::string errlog_file_concat =
      pathing.join_str(logging_src_path, "errlog.txt");

  FILE *std_err_file = freopen(errlog_file_concat.c_str(), "a", stderr);
  if (std_err_file == NULL) {
    fprintf(stderr, "Could not redirect STDERR! -> %s\n", strerror(errno));
  }

  const int ticks_per_frame = (1000.0 / 60);

  sdl2.set_play_state(1);
  while (sdl2.get_play_state()) {
    rend.render_clear(*rend.get_renderer());
    rend.render_bg(*rend.get_renderer(), &rgba);

    frame_start = SDL_GetTicks64();
    int event_return = _key_inputs->poll_events();

    switch (event_return) {
    case QUIT_GAME: {
      sdl2.set_play_state(0);
      break;
    }
    default: {
      break;
    }
    }

    rend.render_present(*rend.get_renderer());

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }
  }

  if (std_out_file != NULL) {
    fclose(std_out_file);
  }

  if (std_err_file != NULL) {
    fclose(std_err_file);
  }

  SDL_Quit();
  return 0;
}
