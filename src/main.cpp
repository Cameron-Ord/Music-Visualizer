#include "../include/audio.hpp"
#include "../include/events.hpp"
#include "../include/fft.hpp"
#include "../include/files.hpp"
#include "../include/font_entity.hpp"
#include "../include/macdefs.hpp"
#include "../include/program_path.hpp"
#include "../include/render_entity.hpp"
#include "../include/sdl2_entity.hpp"
#include "../include/theme.hpp"
#include "../include/window_entity.hpp"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_video.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // instantiate classes
  bool err;

  USERDATA userdata;
  FourierTransform fft;
  SDL2INTERNAL sdl2;
  SDL2Renderer rend;
  SDL2Window win;
  SDL2KeyInputs key;
  ProgramThemes themes;
  ProgramPath pathing;
  ProgramFiles files;
  SDL2Fonts fonts;
  AudioData ad;
  SDL2Audio sdl2_ad;

  userdata.ad = &ad;
  userdata.sdl2_ad = &sdl2_ad;
  userdata.fft = &fft;

  if (!sdl2.initialize_sdl2_video()) {
    fprintf(stdout, "Failed to initialize SDL2 video!\n");
    return 1;
  }

  win.create_window(win.get_window());
  if (win.get_window() == NULL) {
    fprintf(stderr, "Failed to initialize SDL2 window! -> "
                    "WINDOW POINTER WAS NULL\n");
    SDL_Quit();
    return 1;
  }

  WIN_SIZE sizes = sdl2.get_current_window_size(*win.get_window());
  sdl2.set_window_size(sizes);

  rend.set_font_draw_limit(sizes.HEIGHT);
  fonts.set_char_limit(sizes.WIDTH);

  rend.create_renderer(win.get_window(), rend.get_renderer());
  if (*rend.get_renderer() == NULL) {
    fprintf(stderr, "Failed to initialize SDL2 renderer -> "
                    "RENDER POINTER WAS NULL!\n");
    SDL_Quit();
    return 1;
  }

  SDL_SetRenderDrawBlendMode(*rend.get_renderer(), SDL_BLENDMODE_BLEND);

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

  fonts.approximate_size_utf8();

  err = pathing.create_music_source();
  if (!err) {
    fprintf(stderr, "Failed to create or acknowledge "
                    "existence of source "
                    "directories! -> EXIT\n");
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  err = pathing.create_log_directories();
  if (!err) {
    fprintf(stderr, "Failed to create or acknowledge "
                    "existence of logging "
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

  fonts.create_dir_text(*files.retrieve_directories(), *rend.get_renderer(),
                        *themes.get_text(), fonts.get_font_ptr(),
                        rend.get_font_draw_limit());

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
  uint64_t frame_start;
  int frame_time;

  sdl2.set_play_state(true);
  sdl2.set_current_user_state(AT_DIRECTORIES);

  while (sdl2.get_play_state()) {
    rend.render_bg(*rend.get_renderer(), themes.get_secondary());
    rend.render_clear(*rend.get_renderer());

    switch (sdl2_ad.get_stream_flag()) {
    default: {
      break;
    }
    case 1: {
      switch (*sdl2_ad.get_next_song_flag()) {
      case 0: {
        fft.generate_visual();
        break;
      }

      case 1: {
        sdl2_ad.set_flag(0, sdl2_ad.get_next_song_flag());
        key.cycle_down_list(fonts.get_song_vec(rend.get_song_index())->size());
        sdl2_ad.pause_audio();
        sdl2_ad.close_audio_device();
        const std::vector<Text> *f = fonts.get_song_vec(rend.get_song_index());
        const std::string file_name = key.select_element(f);
        const std::string dir_path =
            pathing.join_str(pathing.get_src_path(), pathing.get_opened_dir());
        const std::string file_path = pathing.join_str(dir_path, file_name);
        bool result = ad.read_audio_file(file_path);
        if (result) {
          sdl2_ad.set_audio_spec(&userdata);
          sdl2_ad.open_audio_device();
          sdl2_ad.resume_audio();
          sdl2.set_current_user_state(LISTENING);
        }
        break;
      }
      default: {
        break;
      }
      }
      break;
    }
    }

    switch (sdl2.get_current_user_state()) {
    case AT_DIRECTORIES: {
      rend.set_font_draw_limit(sdl2.get_stored_window_size()->HEIGHT);
      rend.render_set_text(sdl2.get_stored_window_size(),
                           fonts.get_dir_vec(rend.get_dir_index()));
      rend.render_draw_text(*rend.get_renderer(),
                            fonts.get_dir_vec(rend.get_dir_index()));
      rend.render_set_text_bg(sdl2.get_stored_window_size(),
                              fonts.get_dir_vec(rend.get_dir_index()),
                              key.get_cursor_index());
      rend.render_draw_text_bg(*rend.get_renderer(), themes.get_textbg());
      break;
    }

    case AT_SONGS: {
      rend.set_font_draw_limit(sdl2.get_stored_window_size()->HEIGHT);
      rend.render_set_text(sdl2.get_stored_window_size(),
                           fonts.get_song_vec(rend.get_song_index()));
      rend.render_draw_text(*rend.get_renderer(),
                            fonts.get_song_vec(rend.get_song_index()));
      rend.render_set_text_bg(sdl2.get_stored_window_size(),
                              fonts.get_song_vec(rend.get_song_index()),
                              key.get_cursor_index());
      rend.render_draw_text_bg(*rend.get_renderer(), themes.get_textbg());
      break;
    }

    case LISTENING: {
      FData *data = fft.get_data();
      FBuffers *bufs = fft.get_bufs();
      const WIN_SIZE *sizes = sdl2.get_stored_window_size();
      rend.render_draw_bars(&data->output_len, bufs->smear, bufs->smoothed,
                            &sizes->HEIGHT, &sizes->WIDTH, themes.get_primary(),
                            themes.get_textbg(), *rend.get_renderer());
      break;
    }

    default: {
      break;
    }
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      default: {
        break;
      }

      case SDL_WINDOWEVENT: {
        switch (e.window.event) {
        default: {
          break;
        }
        case SDL_WINDOWEVENT_RESIZED: {
          WIN_SIZE ws = sdl2.get_current_window_size(*win.get_window());
          sdl2.set_window_size(ws);
          rend.set_font_draw_limit(ws.HEIGHT);
          fonts.set_char_limit(ws.WIDTH);

          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_SONGS: {
            fonts.create_file_text(*files.retrieve_directory_files(),
                                   *rend.get_renderer(), *themes.get_text(),
                                   fonts.get_font_ptr(),
                                   rend.get_font_draw_limit());
            break;
          }
          case AT_DIRECTORIES: {
            fonts.create_dir_text(*files.retrieve_directories(),
                                  *rend.get_renderer(), *themes.get_text(),
                                  fonts.get_font_ptr(),
                                  rend.get_font_draw_limit());
            break;
          }
          }
          break;
        }

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          WIN_SIZE ws = sdl2.get_current_window_size(*win.get_window());
          sdl2.set_window_size(ws);
          rend.set_font_draw_limit(ws.HEIGHT);
          fonts.set_char_limit(ws.WIDTH);

          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_SONGS: {
            fonts.create_file_text(*files.retrieve_directory_files(),
                                   *rend.get_renderer(), *themes.get_text(),
                                   fonts.get_font_ptr(),
                                   rend.get_font_draw_limit());
            break;
          }
          case AT_DIRECTORIES: {
            fonts.create_dir_text(*files.retrieve_directories(),
                                  *rend.get_renderer(), *themes.get_text(),
                                  fonts.get_font_ptr(),
                                  rend.get_font_draw_limit());
            break;
          }
          }
          break;
        }
        }
        break;
      }

      case SDL_KEYDOWN: {
        switch (e.key.keysym.sym) {
        default: {
          break;
        }

        case LEFT: {
          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_DIRECTORIES: {
            break;
          }
          case AT_SONGS: {
            sdl2.set_current_user_state(AT_DIRECTORIES);
            break;
          }
          case LISTENING: {
            sdl2.set_current_user_state(AT_SONGS);
            break;
          }
          }
          break;
        }

        case RIGHT: {
          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_DIRECTORIES: {
            const std::vector<Text> *d =
                fonts.get_dir_vec(rend.get_dir_index());
            std::string dir_name = key.select_element(d);
            pathing.set_opened_dir(dir_name);
            bool result = files.fill_files(
                pathing.join_str(pathing.get_src_path(), dir_name),
                pathing.return_slash());
            if (files.retrieve_directory_files()->size() > 0 && result) {
              key.reset_cursor_index();
              fonts.create_file_text(*files.retrieve_directory_files(),
                                     *rend.get_renderer(), *themes.get_text(),
                                     fonts.get_font_ptr(),
                                     rend.get_font_draw_limit());
              sdl2.set_current_user_state(AT_SONGS);
            }

            break;
          }
          case AT_SONGS: {
            sdl2_ad.pause_audio();
            sdl2_ad.close_audio_device();
            const std::vector<Text> *f =
                fonts.get_song_vec(rend.get_song_index());
            const std::string file_name = key.select_element(f);
            const std::string dir_path = pathing.join_str(
                pathing.get_src_path(), pathing.get_opened_dir());
            const std::string file_path = pathing.join_str(dir_path, file_name);
            bool result = ad.read_audio_file(file_path);
            if (result) {
              sdl2_ad.set_audio_spec(&userdata);
              sdl2_ad.open_audio_device();
              sdl2_ad.resume_audio();
              sdl2.set_current_user_state(LISTENING);
            }
            break;
          }
          case LISTENING: {
            break;
          }
          }
          break;
        }

        case UP: {

          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_DIRECTORIES: {

            if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
              size_t current_index = rend.get_dir_index();
              size_t vec_size = fonts.get_dir_vec_size();

              if (current_index > 0) {
                rend.set_dir_index(current_index - 1);
              }

              key.reset_cursor_index();
            } else {
              key.cycle_up_list(
                  fonts.get_dir_vec(rend.get_dir_index())->size());
            }
            break;
          }
          case AT_SONGS: {

            if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
              size_t current_index = rend.get_song_index();
              size_t vec_size = fonts.get_song_vec_size();

              if (current_index > 0) {
                rend.set_song_index(current_index - 1);
              }

              key.reset_cursor_index();
            } else {
              key.cycle_up_list(
                  fonts.get_song_vec(rend.get_song_index())->size());
            }
            break;
          }
          }
          break;
        }
        case DOWN: {
          switch (sdl2.get_current_user_state()) {
          default: {
            break;
          }
          case AT_DIRECTORIES: {

            if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
              size_t current_index = rend.get_dir_index();
              size_t vec_size = fonts.get_dir_vec_size();

              if (vec_size - 1 > current_index) {
                rend.set_dir_index(current_index + 1);
              }

              key.reset_cursor_index();
            } else {
              key.cycle_down_list(
                  fonts.get_dir_vec(rend.get_dir_index())->size());
            }

            break;
          }
          case AT_SONGS: {

            if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
              size_t current_index = rend.get_song_index();
              size_t vec_size = fonts.get_song_vec_size();

              if (vec_size - 1 > current_index) {
                rend.set_song_index(current_index + 1);
              }

              key.reset_cursor_index();
            } else {
              key.cycle_down_list(
                  fonts.get_song_vec(rend.get_song_index())->size());
            }
            break;
          }
          }

          break;
        }
        }
        break;
      }

      case SDL_QUIT: {
        sdl2.set_play_state(false);
        break;
      }
      }
    }

    frame_start = SDL_GetTicks64();

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

  fonts.destroy_allocated_fonts();

  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}
