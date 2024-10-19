#include "main.h"
#include "SDL2/SDL_video.h"
#include "audio.h"
#include "audiodefs.h"
#include "filesystem.h"
#include "fontdef.h"
#include "particledef.h"
#include "particles.h"
#include "utils.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef _WIN32
Paths *(*find_directories)(size_t *) = &win_find_directories;
Paths *(*find_files)(size_t *, const char *) = &win_find_files;
#endif

#ifdef __linux__
Paths *(*find_directories)(size_t *) = &unix_find_directories;
Paths *(*find_files)(size_t *, const char *) = &unix_find_files;
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Window win;
Renderer rend;
Font font;
Visualizer vis;
Events key;

SDL_Color primary = {130, 170, 255, 255};
SDL_Color secondary = {68, 66, 103, 255};
SDL_Color background = {41, 45, 62, 255};
SDL_Color text = {103, 110, 149, 255};
SDL_Color text_bg = {113, 124, 180, 255};

int FPS = 60;

int main(int argc, char **argv) {
  scc(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
  scc(TTF_Init());

  if (argc > 1) {
    FPS = atoi(argv[1]);
    if (FPS > 0) {
      if (FPS < 30 || FPS > 360) {
        FPS = 60;
      }
    }
  }

  fprintf(stdout, "Set FPS -> %d\n", FPS);

  srand(time(NULL));

  const char *platform = SDL_GetPlatform();
  if (strcmp(platform, "Linux") != 0 && strcmp(platform, "Windows") != 0) {
    fprintf(stderr, "Wrong platform\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "Starting..\n");

  key.dir_cursor = 0;
  key.file_cursor = 0;
  key.file_list_index = 0;
  key.dir_list_index = 0;

  vis.next_song_flag = 0;
  vis.scrolling = 0;
  vis.quit = false;
  vis.current_state = DIRECTORIES;
  vis.primary = primary;
  vis.secondary = secondary;
  vis.background = background;
  vis.text = text;
  vis.text_bg = text_bg;

  win.w = NULL;
  rend.r = NULL;

  printf("Creating window..\n");
  win.w = (SDL_Window *)scp(SDL_CreateWindow(
      "Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W,
      WIN_H, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));

  printf("Creating renderer..\n");
  rend.r = (SDL_Renderer *)scp(SDL_CreateRenderer(
      win.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

  render_clear();

  win.height = WIN_H;
  win.width = WIN_W;

  font.font = NULL;
  font.char_limit = 0;
  font.size = 16;

  printf("Opening font..\n");

  font.font = (TTF_Font *)scp(TTF_OpenFont(FONT_PATH, font.size));

  rend.title_limit = get_title_limit(win.height);
  font.char_limit = get_char_limit(win.width);

  size_t dir_count = 0;
  size_t file_count = 0;

  printf("Finding directories..\n");
  Paths *dir_contents = find_directories(&dir_count);
  Paths *file_contents = NULL;

  if (!dir_contents) {
    fprintf(stderr, "Failed to read directories! -> EXIT\n");
    exit(EXIT_FAILURE);
  }

  if (dir_contents && dir_count == 0) {
    fprintf(stdout, "No directories!\n");
    free(dir_contents);
    dir_contents = NULL;
  }

  TextBuffer *dir_text_buffer = NULL;
  TextBuffer *file_text_buffer = NULL;

  dir_text_buffer = create_fonts(dir_contents, &dir_count);
  if (!dir_text_buffer) {
    fprintf(stdout, "Fonts weren't created!\n");
    dir_text_buffer = NULL;
  }

  AudioDataContainer *adc = calloc(1, sizeof(AudioDataContainer));
  if (!adc) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  FFTBuffers *f_buffers = calloc(1, sizeof(FFTBuffers));
  if (!f_buffers) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  FFTData *f_data = calloc(1, sizeof(FFTData));
  if (!f_data) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  adc->next = f_buffers;
  f_buffers->next = f_data;
  f_data->next = adc;

  size_t particle_buffer_size = 0;
  ParticleTrio *particle_buffer =
      allocate_particle_buffer(&particle_buffer_size);
  if (!particle_buffer) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  calculate_window(f_data->hamming_values);

  scc(SDL_SetRenderDrawBlendMode(rend.r, SDL_BLENDMODE_BLEND));
  SDL_EnableScreenSaver();
  SDL_ShowWindow(win.w);

  const int ticks_per_frame = (1000.0 / FPS);
  uint64_t frame_start;
  int frame_time;

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();

    render_bg();
    render_clear();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      default:
        break;

      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
          SDL_GetWindowSize(win.w, &win.width, &win.height);
          rend.title_limit = get_title_limit(win.height);
          font.char_limit = get_char_limit(win.width);

          TextBuffer *original_dir = dir_text_buffer;
          TextBuffer *original_files = file_text_buffer;

          dir_text_buffer = create_fonts(dir_contents, &dir_count);
          if (!dir_text_buffer) {
            dir_text_buffer = original_dir;
          } else {
            if (original_dir) {
              free(original_dir);
            }
          }

          file_text_buffer = create_fonts(file_contents, &file_count);
          if (!file_text_buffer) {
            file_text_buffer = original_files;
          } else {
            if (original_files) {
              free(original_files);
            }
          }

        } break;

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          SDL_GetWindowSize(win.w, &win.width, &win.height);
          rend.title_limit = get_title_limit(win.height);
          font.char_limit = get_char_limit(win.width);

          TextBuffer *original_dir = dir_text_buffer;
          TextBuffer *original_files = file_text_buffer;

          dir_text_buffer = create_fonts(dir_contents, &dir_count);
          if (!dir_text_buffer) {
            dir_text_buffer = original_dir;
          } else {
            if (original_dir) {
              free(original_dir);
            }
          }

          file_text_buffer = create_fonts(file_contents, &file_count);
          if (!file_text_buffer) {
            file_text_buffer = original_files;
          } else {
            if (original_files) {
              free(original_files);
            }
          }

        } break;
        }
      } break;

      case SDL_KEYUP: {
        switch (event.key.keysym.sym) {
        default:
          break;
        case SDLK_LEFT: {
          vis.scrolling = 0;
        } break;

        case SDLK_RIGHT: {
          vis.scrolling = 0;
        } break;
        }
      } break;

      case SDL_KEYDOWN: {
        switch (vis.current_state) {
        default:
          break;

        case PLAYBACK: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_p: {
            if (vis.dev) {
              switch (SDL_GetAudioDeviceStatus(vis.dev)) {
              default:
                break;

              case SDL_AUDIO_STOPPED: {

              } break;

              case SDL_AUDIO_PAUSED: {
                resume_device();
              } break;

              case SDL_AUDIO_PLAYING: {
                pause_device();
              } break;
              }
            }
          } break;
          case SDLK_LEFT: {
            if (event.key.keysym.mod & KMOD_SHIFT) {
              vis.scrolling = 1;

              int pos_cpy = adc->position - M_BUF_SIZE;
              if (pos_cpy < 0) {
                pos_cpy = 0;
              }

              adc->position = pos_cpy;
            } else {
              if (file_count > 0)
                vis.current_state = SONGS;
            }
          } break;

          case SDLK_RIGHT: {
            if (event.key.keysym.mod & KMOD_SHIFT) {
              vis.scrolling = 1;

              int pos_cpy = adc->position + M_BUF_SIZE;
              if (pos_cpy > (int)adc->length) {
                pos_cpy = adc->length - M_BUF_SIZE;
              }

              adc->position = pos_cpy;
            } else {
              if (dir_count > 0)
                vis.current_state = DIRECTORIES;
            }
          } break;
          }
        } break;

        case SONGS: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_LEFT: {
            if (dir_count > 0)
              vis.current_state = DIRECTORIES;
          } break;

          case SDLK_RIGHT: {
            if (vis.stream_flag)
              vis.current_state = PLAYBACK;
          } break;

          case SDLK_UP: {
            nav_up(&key.file_cursor, &file_count);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.file_cursor, &file_count);
          } break;

          case SDLK_SPACE: {
            const char *search_key =
                file_text_buffer[key.file_cursor].text->name;
            const char *path_str = find_pathstr(search_key, file_contents);

            KILL_PARTICLES(particle_buffer, particle_buffer_size);

            if (read_audio_file(path_str, adc)) {
              if (load_song(adc)) {
                vis.current_state = PLAYBACK;
              } else {
                vis.current_state = SONGS;
              }
            } else {
              pause_device();
              SDL_CloseAudioDevice(vis.dev);
              vis.current_state = SONGS;
            }

          } break;
          }

        } break;

        case DIRECTORIES: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_LEFT: {
            if (vis.stream_flag) {
              vis.current_state = PLAYBACK;
            }
          } break;

          case SDLK_RIGHT: {
            if (file_count > 0)
              vis.current_state = SONGS;
          } break;

          case SDLK_UP: {
            nav_up(&key.dir_cursor, &dir_count);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.dir_cursor, &dir_count);
          } break;

          case SDLK_SPACE: {
            // I am overwritting possibly priorly allocated heap objects here so
            // there are some crucial steps to make sure things get cleaned up
            // correctly. Note that said cleanup isn't fully implemented yet as
            // the allocated objects do contain heap allocated objects, so make
            // sure to get that stuff done.

            const char *search_key = dir_text_buffer[key.dir_cursor].text->name;
            const char *path_str = find_pathstr(search_key, dir_contents);

            // Save the original pointer and file count
            const size_t original_file_count = file_count;
            Paths *original_file_contents = file_contents;

            // Call the function to use a platform specific API to read from a
            // directory
            file_contents = find_files(&file_count, path_str);

            if (file_count > 0 && file_contents) {
              TextBuffer *original_text_buffer = file_text_buffer;

              file_text_buffer = create_fonts(file_contents, &file_count);
              if (file_text_buffer) {
                // Free the original if it was allocated.
                if (original_file_contents) {
                  free(original_file_contents);
                }

                // Free the original if it was allocated.
                if (original_text_buffer) {
                  free(original_text_buffer);
                }

                key.file_cursor = 0;
                vis.current_state = SONGS;
              } else {
                free(file_contents);

                // Set the pointers back to the original pointers.
                file_text_buffer = original_text_buffer;
                file_contents = original_file_contents;
                file_count = original_file_count;
              }
            } else {
              // Free the newly allocated Paths struct (if it is not NULL)
              if (file_contents) {
                free(file_contents);
              }

              // Set the pointers back to the original pointers.
              file_contents = original_file_contents;
              file_count = original_file_count;
            }
          } break;
          }
        } break;
        }
      } break;

      case SDL_QUIT: {
        SDL_CloseAudioDevice(vis.dev);
        vis.quit = true;

      } break;
      }
    }

    switch (vis.current_state) {
    default:
      break;

    case PLAYBACK: {
      if (!vis.next_song_flag) {
        float tmp[M_BUF_SIZE];
        memcpy(tmp, f_buffers->fft_in, sizeof(float) * M_BUF_SIZE);
        hamming_window(f_buffers->fft_in, f_data->hamming_values,
                       f_buffers->windowed);
        recursive_fft(f_buffers->windowed, 1, f_buffers->out_raw, M_BUF_SIZE);
        extract_frequencies(f_buffers);
        freq_bin_algo(adc->SR, f_buffers->extracted);
        squash_to_log(f_buffers, f_data);
        visual_refine(f_buffers, f_data);
      }
      if (vis.next_song_flag && !vis.scrolling) {

        nav_down(&key.file_cursor, &file_count);
        const char *search_key = file_text_buffer[key.file_cursor].text->name;
        const char *path_str = find_pathstr(search_key, file_contents);

        KILL_PARTICLES(particle_buffer, particle_buffer_size);

        if (read_audio_file(path_str, adc)) {
          if (load_song(adc)) {
            vis.current_state = PLAYBACK;
          } else {
            vis.current_state = SONGS;
          }
        } else {
          pause_device();
          SDL_CloseAudioDevice(vis.dev);
          vis.current_state = SONGS;
        }
      }
    } break;
    }

    switch (vis.current_state) {
    default:
      break;

    case PLAYBACK: {
      if (f_data->output_len > 0) {
        render_draw_music(f_buffers->smear, f_buffers->smoothed,
                          &f_data->output_len, particle_buffer);
      }
    } break;

    case SONGS: {
      assert(file_text_buffer != NULL);
      render_draw_text(file_text_buffer, &file_count, &key.file_cursor);
    } break;

    case DIRECTORIES: {
      assert(file_text_buffer != NULL);
      render_draw_text(dir_text_buffer, &dir_count, &key.dir_cursor);
    } break;
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  if (particle_buffer) {
    free(particle_buffer);
  }

  if (dir_contents) {
    for (size_t i = 0; i < dir_count; i++) {
      if (dir_contents[i].name) {
        free(dir_contents[i].name);
      }

      if (dir_contents[i].path) {
        free(dir_contents[i].path);
      }
    }

    free(dir_contents);
  }

  //  if (file_contents) {
  //  for (size_t i = 0; i < file_count; i++) {
  //  if (file_contents[i].name) {
  //   free(file_contents[i].name);
  // }

  // if (file_contents[i].path) {
  //  free(file_contents[i].path);
  // }
  //}

  // free(file_contents);
  // }

  if (adc) {
    if (adc->buffer) {
      free(adc->buffer);
    }
    free(adc);
  }

  if (f_buffers) {
    free(f_buffers);
  }

  if (f_data) {
    free(f_data);
  }

  if (rend.r) {
    SDL_DestroyRenderer(rend.r);
  }

  if (win.w) {
    SDL_DestroyWindow(win.w);
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}

int get_char_limit(int width) { return MIN(175, MAX(8, (width - 200) / 18)); }

int get_title_limit(int height) { return MIN(6, MAX(1, (height - 200) / 22)); }

void *scp(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "SDL failed to create PTR! -> %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return ptr;
}

int scc(int code) {
  if (code < 0) {
    fprintf(stderr, "SDL code execution failed! -> %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return code;
}
