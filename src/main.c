#include "main.h"
#include "SDL2/SDL_audio.h"
#include "audio.h"
#include "audiodefs.h"
#include "filesystem.h"
#include "fontdef.h"
#include "particledef.h"
#include "particles.h"
#include "utils.h"
#include <math.h>
#include <time.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifdef _WIN32
Paths *(*find_directories)(size_t *) = &win_find_directories;
Paths *(*find_files)(size_t *, char *) = &win_find_files;
#endif

#ifdef __linux__
Paths *(*find_directories)(size_t *) = &unix_find_directories;
Paths *(*find_files)(size_t *, char *) = &unix_find_files;
#endif

#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv) {
  scc(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
  scc(TTF_Init());

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
      WIN_H, SDL_WINDOW_RESIZABLE));

  printf("Creating renderer..\n");
  rend.r = (SDL_Renderer *)scp(SDL_CreateRenderer(
      win.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

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

  size_t d_list_size = 1;
  size_t d_subbuf_size = rend.title_limit;

  size_t f_list_size = 1;
  size_t f_subbuf_size = rend.title_limit;

  TextBuffer *file_text_list = NULL;
  TextBuffer *dir_text_list =
      create_fonts(dir_contents, dir_count, &d_list_size, &d_subbuf_size);
  if (!dir_text_list) {
    fprintf(stderr, "Text creation failed! -> EXIT\n");
    exit(EXIT_FAILURE);
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

  const int ticks_per_frame = (1000.0 / 60);
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

          d_subbuf_size = get_title_limit(win.height);
          f_subbuf_size = get_title_limit(win.height);
          if (dir_count > 0) {
            dir_text_list = create_fonts(dir_contents, dir_count, &d_list_size,
                                         &d_subbuf_size);
            if (!dir_text_list) {
              fprintf(stderr, "Could not create fonts!\n");
              exit(EXIT_FAILURE);
            }
          }

          if (file_count > 0) {
            file_text_list = create_fonts(file_contents, file_count,
                                          &f_list_size, &f_subbuf_size);
            if (!file_text_list) {
              fprintf(stderr, "Could not create fonts!\n");
              exit(EXIT_FAILURE);
            }
          }
        } break;

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          SDL_GetWindowSize(win.w, &win.width, &win.height);
          d_subbuf_size = get_title_limit(win.height);
          f_subbuf_size = get_title_limit(win.height);
          if (dir_count > 0) {
            dir_text_list = create_fonts(dir_contents, dir_count, &d_list_size,
                                         &d_subbuf_size);
            if (!dir_text_list) {
              fprintf(stderr, "Could not create fonts!\n");
              exit(EXIT_FAILURE);
            }
          }

          if (file_count > 0) {
            file_text_list = create_fonts(file_contents, file_count,
                                          &f_list_size, &f_subbuf_size);

            if (!file_text_list) {
              fprintf(stderr, "Could not create fonts!\n");
              exit(EXIT_FAILURE);
            }
          }
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
          case SDLK_LEFT: {
            if (file_text_list) {
              vis.current_state = SONGS;
            }
          } break;

          case SDLK_RIGHT: {
            if (dir_text_list) {
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
            if (dir_text_list) {
              vis.current_state = DIRECTORIES;
            }
          } break;

          case SDLK_RIGHT: {
            if (vis.stream_flag) {
              vis.current_state = PLAYBACK;
            }
          } break;

          case SDLK_UP: {
            if (file_text_list) {

              TextBuffer *list = file_text_list;
              size_t full = file_count;
              size_t *cursor = &key.file_cursor;
              size_t *list_index = &key.file_list_index;
              size_t list_size = f_list_size;

              NavListArgs list_args = {.list = list,
                                       .max_len = full,
                                       .cursor = cursor,
                                       .list_index = list_index,
                                       .list_size = list_size};

              nav_up(&list_args);
            }
          } break;

          case SDLK_DOWN: {
            if (file_text_list) {
              TextBuffer *list = file_text_list;
              size_t full = file_count;
              size_t *cursor = &key.file_cursor;
              size_t *list_index = &key.file_list_index;
              size_t list_size = f_list_size;

              NavListArgs list_args = {.list = list,
                                       .max_len = full,
                                       .cursor = cursor,
                                       .list_index = list_index,
                                       .list_size = list_size};

              nav_down(&list_args);
            }

          } break;

          case SDLK_SPACE: {
            if (file_text_list && key.file_list_index < f_list_size) {
              const Text *text_buf = file_text_list[key.file_list_index].buf;
              const char *search_key = text_buf[key.file_cursor].name;
              if (file_contents && adc && search_key) {
                fprintf(stdout, "Searching file -> %s\n", search_key);
                SDL_CloseAudioDevice(vis.dev);
                if (read_audio_file(find_pathstr(search_key, file_contents),
                                    adc)) {
                  if (!load_song(adc)) {
                    fprintf(stderr,
                            "Failed to load song! CURRENT ERRNO -> %s\n",
                            strerror(errno));
                    fprintf(stderr, "Failed to load song! SDL ERROR -> %s\n",
                            SDL_GetError());
                  }

                  vis.current_state = PLAYBACK;
                }
              }
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
            if (file_text_list) {
              vis.current_state = SONGS;
            }
          } break;

          case SDLK_UP: {
            if (dir_text_list) {
              TextBuffer *list = dir_text_list;
              size_t full = dir_count;
              size_t *cursor = &key.dir_cursor;
              size_t *list_index = &key.dir_list_index;
              size_t list_size = d_list_size;

              NavListArgs list_args = {.list = list,
                                       .max_len = full,
                                       .cursor = cursor,
                                       .list_index = list_index,
                                       .list_size = list_size};

              nav_up(&list_args);
            }
          } break;

          case SDLK_DOWN: {
            if (dir_text_list) {
              TextBuffer *list = dir_text_list;
              size_t full = dir_count;
              size_t *cursor = &key.dir_cursor;
              size_t *list_index = &key.dir_list_index;
              size_t list_size = d_list_size;

              NavListArgs list_args = {.list = list,
                                       .max_len = full,
                                       .cursor = cursor,
                                       .list_index = list_index,
                                       .list_size = list_size};

              nav_down(&list_args);
            }
          } break;

          case SDLK_SPACE: {
            if (dir_text_list && dir_contents) {
              const Text *text_buf = dir_text_list[key.dir_list_index].buf;
              if(text_buf){
              const char *search_key = text_buf[key.dir_cursor].name;
              if (search_key) {
                file_contents = find_files(
                    &file_count, find_pathstr(search_key, dir_contents));
                if (file_contents) {
                  if (file_text_list) {
                    if (file_text_list->buf) {
                      free(file_text_list->buf);
                    }
                    free(file_text_list);
                    file_text_list = NULL;
                  }
                  file_text_list = create_fonts(file_contents, file_count,
                                                &f_list_size, &f_subbuf_size);
                  if (file_text_list) {
                    vis.current_state = SONGS;
                    key.file_list_index = 0;
                    key.file_cursor = 0;
                  }
                }
              }
              }
            }

          } break;
          }
        } break;
        }
      } break;

      case SDL_QUIT: {
        vis.quit = true;

      } break;
      }
    }

    switch (vis.current_state) {
    default:
      break;

    case PLAYBACK: {
      if (vis.stream_flag) {
        if (adc->position < adc->length) {
          hamming_window(f_buffers->fft_in, f_data->hamming_values,
                         f_buffers->windowed);
          recursive_fft(f_buffers->windowed, 1, f_buffers->out_raw, M_BUF_SIZE);
          extract_frequencies(f_buffers);
          freq_bin_algo(adc->SR, f_buffers->extracted);
          squash_to_log(f_buffers, f_data);
          visual_refine(f_buffers, f_data);
        }
      }
      if (!vis.stream_flag) {
        if (file_text_list) {
          TextBuffer *list = file_text_list;
          size_t full = file_count;
          size_t *cursor = &key.file_cursor;
          size_t *list_index = &key.file_list_index;
          size_t list_size = f_list_size;

          NavListArgs list_args = {.list = list,
                                   .max_len = full,
                                   .cursor = cursor,
                                   .list_index = list_index,
                                   .list_size = list_size};

          nav_down(&list_args);

          const Text *text_buf = file_text_list[key.file_list_index].buf;
          if(text_buf){
            const char *search_key = text_buf[key.file_cursor].name;
            if (file_contents && adc && search_key) {
              SDL_CloseAudioDevice(vis.dev);
              if (read_audio_file(find_pathstr(search_key, file_contents), adc)) {
                if (!load_song(adc)) {
                  fprintf(stderr, "Failed to load song! CURRENT ERRNO -> %s\n",
                          strerror(errno));
                  fprintf(stderr, "Failed to load song! SDL ERROR -> %s\n",
                          SDL_GetError());
                }
              }
            }
          }
        }
      }
    } break;
    }

    switch (vis.current_state) {
    default:
      break;

    case PLAYBACK: {
      if (f_data->output_len > 0 && vis.stream_flag) {
        render_draw_music(f_buffers->smear, f_buffers->smoothed,
                          &f_data->output_len, particle_buffer);
      }
    } break;

    case SONGS: {
      if (file_text_list && key.file_list_index < f_list_size &&
          key.file_cursor < f_subbuf_size) {
        render_draw_text(&file_text_list[key.file_list_index],
                         &key.file_cursor);
      }
    } break;

    case DIRECTORIES: {
      if (dir_text_list && key.dir_list_index < d_list_size &&
          key.dir_cursor < d_subbuf_size) {
        render_draw_text(&dir_text_list[key.dir_list_index], &key.dir_cursor);
      }

    } break;
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  if (SDL_GetAudioDeviceStatus(vis.dev) > 0) {
    SDL_CloseAudioDevice(vis.dev);
  }

  if (particle_buffer) {
    free(particle_buffer);
  }

  if (dir_text_list) {
    if (dir_text_list->buf) {
      free(dir_text_list->buf);
    }
    free(dir_text_list);
  }

  if (file_text_list) {
    if (file_text_list->buf) {
      free(file_text_list->buf);
    }
    free(file_text_list);
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

  if (file_contents) {
    for (size_t i = 0; i < file_count; i++) {
      if (file_contents[i].name) {
        free(file_contents[i].name);
      }

      if (file_contents[i].path) {
        free(file_contents[i].path);
      }
    }

    free(file_contents);
  }

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

int get_char_limit(int width) { return MIN(175, MAX(8, (width - 200) / 12)); }

int get_title_limit(int height) { return MIN(24, MAX(1, (height - 100) / 32)); }

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
