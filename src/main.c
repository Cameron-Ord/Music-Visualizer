#include "main.h"
#include "audio.h"
#include "filesystem.h"
#include "fontdef.h"
#include "particles.h"
#include "utils.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>

#ifdef LUA_FLAG
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef _WIN32
#define HOME "USERPROFILE"
#define ASSETS_DIR "\\Documents\\share\\MVis\\"
Paths *(*find_directories)(size_t *) = &win_find_directories;
Paths *(*find_files)(size_t *, const char *) = &win_find_files;
#endif

#ifdef __linux__
#define HOME "HOME"
#define ASSETS_DIR "/.local/share/MVis/"
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

SDL_Color primary = {112, 176, 255, 255};
SDL_Color secondary = {122, 248, 202, 255};  // Green
SDL_Color background = {34, 36, 54, 255};    // Dark Grey
SDL_Color secondary_bg = {47, 51, 77, 255};  // Lighter-Dark Grey
SDL_Color foreground = {200, 211, 245, 255}; // White - Grey
SDL_Color text = {130, 139, 184, 255};       // Light Grey
SDL_Color tertiary = {126, 142, 218, 255};   // Blue-ish Grey

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

  srand(time(NULL));

  const char *platform = SDL_GetPlatform();
  if (strcmp(platform, "Linux") != 0 && strcmp(platform, "Windows") != 0) {
    fprintf(stderr, "Wrong platform\n");
    exit(EXIT_FAILURE);
  }

  char *home = getenv(HOME);
  if (!home) {
    fprintf(stderr, "Failed to get home environment! -> %s\n", strerror(errno));
    return 1;
  }

  fprintf(stdout, "Starting..\n");

  key.dir_cursor = 0;
  key.file_cursor = 0;
  key.search_cursor = 0;
  key.file_list_index = 0;
  key.dir_list_index = 0;

  // Assign default values
  vis.home = home;
  vis.smearing = 6;
  vis.smoothing = 8;
  vis.target_frames = FPS;
  vis.draw_state = FREQ_DOMAIN;
  vis.next_song_flag = 0;
  vis.scrolling = 0;
  vis.quit = false;
  vis.current_state = DIRECTORIES;
  vis.last_state = vis.current_state;
  vis.primary = primary;
  vis.secondary = secondary;
  vis.background = background;
  vis.secondary_bg = secondary_bg;
  vis.text = text;
  vis.foreground = foreground;
  vis.tertiary = tertiary;

  win.w = NULL;
  rend.r = NULL;

  printf("Creating window..\n");
  win.w = (SDL_Window *)scp(SDL_CreateWindow(
      "Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W,
      WIN_H, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));

  printf("Creating renderer..\n");
  rend.r = (SDL_Renderer *)scp(SDL_CreateRenderer(
      win.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

  win.height = WIN_H;
  win.width = WIN_W;

  font.font = NULL;
  font.char_limit = 0;
  font.size = 16;

  int written = 0;
  char *path_buffer = NULL;
  size_t path_size = 0;

#ifdef LUA_FLAG
  const char *config_name = "config.lua";
  path_size =
      get_length(3, strlen(config_name), strlen(home), strlen(ASSETS_DIR));

  path_buffer = malloc(path_size + 1);
  if (!path_buffer) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return 1;
  }

  written = snprintf(path_buffer, path_size + 1, "%s%s%s", home, ASSETS_DIR,
                     config_name);
  if (written <= 0) {
    fprintf(stderr, "snprintf failed! -> %s\n", strerror(errno));
    return 1;
  }

  printf("Lua config path -> %s\n", path_buffer);

  lua_State *L = luaL_newstate();
  assert(L != NULL);
  luaL_openlibs(L);

  bool lua_failed = false;

  if (luaL_dostring(L, "print('Lua state initialized!')") != LUA_OK) {
    fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_failed = true;
  }

  if (luaL_dofile(L, path_buffer) != LUA_OK) {
    fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_failed = true;
  }

  free(path_buffer);

  if (!lua_failed) {
    lua_getglobal(L, "Config");
    lua_getfield(L, -1, "FPS");
    vis.target_frames = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "smearing");
    vis.smearing = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "smoothing");
    vis.smoothing = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "freq_draw_type");
    vis.draw_state = lua_tointeger(L, -1);
    lua_pop(L, 1);

    const char *fields[] = {"primary",    "secondary",    "tertiary",
                            "background", "secondary_bg", "foreground",
                            "text"};

    SDL_Color *color_ptrs[] = {
        &vis.primary,      &vis.secondary,  &vis.tertiary, &vis.background,
        &vis.secondary_bg, &vis.foreground, &vis.text};

    const size_t col_size = sizeof(color_ptrs) / sizeof(color_ptrs[0]);
    for (size_t i = 0; i < col_size; i++) {
      lua_getfield(L, -1, fields[i]);
      for (size_t j = 0; j < 4; j++) {

        /*
          int lua_gettable (lua_State *L, int index);

          Pushes onto the stack the value t[k], where t is the value at the
          given index and k is the value at the top of the stack.

          This function pops the key from the stack, pushing the resulting value
          in its place. As in Lua, this function may trigger a metamethod for
          the "index" event (see §2.4).
        */

        // push index onto the stack
        lua_pushinteger(L, j + 1);
        // get the j-th value from the table (array)
        lua_gettable(L, -2);
        switch (j) {
        case 0: {
          color_ptrs[i]->r = lua_tointeger(L, -1);
        } break;

        case 1: {
          color_ptrs[i]->g = lua_tointeger(L, -1);
        } break;

        case 2: {
          color_ptrs[i]->b = lua_tointeger(L, -1);
        } break;

        case 3: {
          color_ptrs[i]->a = lua_tointeger(L, -1);
        } break;
        }

        lua_pop(L, 1);
      }

      lua_pop(L, 1);
    }

    lua_pop(L, 1);
  }

  if (L && !lua_failed) {
    lua_close(L);
  }

#endif
  fprintf(stdout, "FPS TARGET -> %d\n", vis.target_frames);

  written = 0;
  path_buffer = NULL;
  path_size = 0;

  printf("Opening font..\n");
  const char *font_name = "dogicapixel.ttf";
  path_size =
      get_length(3, strlen(font_name), strlen(home), strlen(ASSETS_DIR));

  path_buffer = malloc(path_size + 1);
  if (!path_buffer) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return 1;
  }

  written = snprintf(path_buffer, path_size + 1, "%s%s%s", home, ASSETS_DIR,
                     font_name);
  if (written <= 0) {
    fprintf(stderr, "snprintf failed! -> %s\n", strerror(errno));
    return 1;
  }

  printf("Font path -> %s\n", path_buffer);
  font.font = (TTF_Font *)scp(TTF_OpenFont(path_buffer, font.size));

  free(path_buffer);

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

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  size_t input_buf_size = DEFAULT_INPUT_BUFFER_SIZE;
  size_t input_buf_position = 0;
  char *text_input_buffer = malloc(input_buf_size + 1);
  if (!text_input_buffer) {
    ERRNO_CALLBACK("Failed to allocate pointer!", strerror(errno));
    exit(EXIT_FAILURE);
  }

  text_input_buffer[0] = '\0';
  SDL_StopTextInput();

  size_t filter_count = 0;
  TextBuffer *filtered_tb = NULL;
  Text *search_text = NULL;

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();

    render_bg();
    render_clear();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      default:
        break;

      case SDL_TEXTINPUT: {
        switch (vis.current_state) {
        default:
          break;

        case SEARCHING_DIRS: {
          char_buf_insert(event.text.text, &text_input_buffer,
                          &input_buf_position, &input_buf_size, &search_text);
        } break;

        case SEARCHING_SONGS: {
          char_buf_insert(event.text.text, &text_input_buffer,
                          &input_buf_position, &input_buf_size, &search_text);
        } break;
        }
      } break;

      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
          window_resized();
          dir_text_buffer = font_swap_pointer(dir_text_buffer, &dir_count,
                                              dir_contents, filtered_tb);

          file_text_buffer = font_swap_pointer(file_text_buffer, &file_count,
                                               file_contents, filtered_tb);

        } break;

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          window_resized();
          dir_text_buffer = font_swap_pointer(dir_text_buffer, &dir_count,
                                              dir_contents, filtered_tb);

          file_text_buffer = font_swap_pointer(file_text_buffer, &file_count,
                                               file_contents, filtered_tb);

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

        case DISPLAY_SEARCH: {

          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_ESCAPE: {
            vis.current_state = vis.last_state;
            if (filtered_tb) {
              free(filtered_tb);
              filtered_tb = NULL;
            }
            SDL_StartTextInput();

          } break;

          case SDLK_UP: {
            nav_up(&key.search_cursor, &filter_count);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.search_cursor, &filter_count);
          } break;

          case SDLK_SPACE: {
            switch (vis.last_state) {
            default:
              break;
            case SEARCHING_DIRS: {
              if (dir_text_buffer) {
                fill_file_contents(
                    &file_contents, &file_text_buffer, &file_count,
                    seek_path(filtered_tb, &key.search_cursor, dir_contents),
                    find_files);
              }
            } break;

            case SEARCHING_SONGS: {
              if (file_text_buffer) {
                KILL_PARTICLES(particle_buffer, particle_buffer_size);
                select_file(adc, seek_path(filtered_tb, &key.search_cursor,
                                           file_contents));
              }
            } break;
            }
          } break;
          }

        } break;

        case SEARCHING_DIRS: {
          if (event.key.keysym.sym == SDLK_RETURN) {
            if (filtered_tb) {
              // Since filtered_tb just holds Text struct pointers that were
              // heap allocated earlier, we just free the filtered buffer itself
              // and not the underlying data it holds (that would be bad)
              free(filtered_tb);
              filtered_tb = NULL;
            }

            filtered_tb = malloc(sizeof(TextBuffer) * dir_count);
            if (!filtered_tb) {
              ERRNO_CALLBACK("malloc failed!", strerror(errno));
              exit(EXIT_FAILURE);
            }

            filter_count = do_search(text_input_buffer, &dir_count,
                                     dir_text_buffer, &filtered_tb);
            if (filter_count <= 0) {
              free(filtered_tb);
              filtered_tb = NULL;
            }

            if (filter_count > 0) {
              TextBuffer *tmp =
                  realloc(filtered_tb, sizeof(TextBuffer) * filter_count);
              if (!tmp) {
                ERRNO_CALLBACK("realloc failed!", strerror(errno));
                exit(EXIT_FAILURE);
              }
              filtered_tb = tmp;
            }

            if (filtered_tb && filter_count > 0) {
              vis.last_state = vis.current_state;
              vis.current_state = DISPLAY_SEARCH;
              SDL_StopTextInput();
            }
          }

          if (event.key.keysym.sym == SDLK_BACKSPACE) {
            remove_char(&text_input_buffer, &input_buf_position,
                        &input_buf_position);

            search_text = destroy_search_text(search_text);
            search_text = create_search_text(text_input_buffer, &input_buf_size,
                                             &input_buf_position);
          }

          if (event.key.keysym.sym == SDLK_ESCAPE) {
            vis.last_state = vis.current_state;
            vis.current_state = DIRECTORIES;
            int is_active = SDL_IsTextInputActive();
            if (is_active) {
              SDL_StopTextInput();
            }
          }
        } break;

        case SEARCHING_SONGS: {
          if (event.key.keysym.sym == SDLK_RETURN) {
            if (filtered_tb) {
              // Since filtered_tb just holds Text struct pointers that were
              // heap allocated earlier, we just free the filtered buffer itself
              // and not the underlying data it holds (that would be bad)
              free(filtered_tb);
              filtered_tb = NULL;
            }

            filtered_tb = malloc(sizeof(TextBuffer) * file_count);
            if (!filtered_tb) {
              ERRNO_CALLBACK("malloc failed!", strerror(errno));
              exit(EXIT_FAILURE);
            }

            filter_count = do_search(text_input_buffer, &file_count,
                                     file_text_buffer, &filtered_tb);
            if (filter_count <= 0) {
              free(filtered_tb);
              filtered_tb = NULL;
            }

            if (filter_count > 0) {
              TextBuffer *tmp =
                  realloc(filtered_tb, sizeof(TextBuffer) * filter_count);
              if (!tmp) {
                ERRNO_CALLBACK("realloc failed!", strerror(errno));
                exit(EXIT_FAILURE);
              }
              filtered_tb = tmp;
            }

            if (filtered_tb && filter_count > 0) {
              vis.last_state = vis.current_state;
              vis.current_state = DISPLAY_SEARCH;
              SDL_StopTextInput();
            }
          }

          if (event.key.keysym.sym == SDLK_BACKSPACE) {
            remove_char(&text_input_buffer, &input_buf_position,
                        &input_buf_position);

            search_text = destroy_search_text(search_text);
            search_text = create_search_text(text_input_buffer, &input_buf_size,
                                             &input_buf_position);
          }

          if (event.key.keysym.sym == SDLK_ESCAPE) {
            vis.last_state = vis.current_state;
            vis.current_state = SONGS;
            int is_active = SDL_IsTextInputActive();
            if (is_active) {
              SDL_StopTextInput();
            }
          }
        } break;

        case PLAYBACK: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_s: {
            if (vis.dev) {
              switch (SDL_GetAudioDeviceStatus(vis.dev)) {
              default:
                break;

              case SDL_AUDIO_PLAYING: {
                SDL_CloseAudioDevice(vis.dev);
                vis.stream_flag = false;
                vis.last_state = vis.current_state;
                vis.current_state = SONGS;
                break;
              }
              }
            }
          } break;

          case SDLK_p: {
            if (vis.dev) {
              switch (SDL_GetAudioDeviceStatus(vis.dev)) {
              default:
                break;

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
            if (vis.dev) {
              switch (SDL_GetAudioDeviceStatus(vis.dev)) {
              default:
                break;

              case SDL_AUDIO_PLAYING: {
                if (event.key.keysym.mod & KMOD_SHIFT) {
                  vis.scrolling = 1;

                  int pos_cpy = adc->position - (1 << 15);
                  if (pos_cpy < 0) {
                    pos_cpy = 0;
                  }

                  adc->position = pos_cpy;
                } else {
                  if (file_count > 0) {
                    vis.current_state = SONGS;
                    vis.last_state = vis.current_state;
                  }
                }
              } break;

              case SDL_AUDIO_PAUSED: {
                if (event.key.keysym.mod & KMOD_SHIFT) {
                  resume_device();
                  vis.scrolling = 1;

                  int pos_cpy = adc->position - (1 << 15);
                  if (pos_cpy < 0) {
                    pos_cpy = 0;
                  }

                  adc->position = pos_cpy;
                } else {
                  if (file_count > 0) {
                    vis.current_state = SONGS;
                    vis.last_state = vis.current_state;
                  }
                }

              } break;
              }
            }

          } break;

          case SDLK_RIGHT: {
            if (vis.dev) {
              switch (SDL_GetAudioDeviceStatus(vis.dev)) {
              default:
                break;

              case SDL_AUDIO_PLAYING: {
                if (event.key.keysym.mod & KMOD_SHIFT) {
                  vis.scrolling = 1;

                  int pos_cpy = adc->position + (1 << 15);
                  if (pos_cpy > (int)adc->length) {
                    pos_cpy = adc->length - M_BUF_SIZE;
                  }

                  adc->position = pos_cpy;
                } else {
                  if (dir_count > 0) {
                    vis.current_state = DIRECTORIES;
                    vis.last_state = vis.current_state;
                  }
                }

              } break;

              case SDL_AUDIO_PAUSED: {
                if (event.key.keysym.mod & KMOD_SHIFT) {
                  resume_device();
                  vis.scrolling = 1;

                  int pos_cpy = adc->position + (1 << 15);
                  if (pos_cpy > (int)adc->length) {
                    pos_cpy = adc->length - M_BUF_SIZE;
                  }

                  adc->position = pos_cpy;
                } else {
                  if (dir_count > 0) {
                    vis.current_state = DIRECTORIES;
                    vis.last_state = vis.current_state;
                  }
                }

              } break;
              }
            }

          } break;
          }
        } break;

        case SONGS: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_s: {
            vis.last_state = vis.current_state;
            vis.current_state = SEARCHING_SONGS;
            input_buf_position = 0;
            text_input_buffer[0] = '\0';
            search_text = destroy_search_text(search_text);
            search_text = create_search_text(text_input_buffer, &input_buf_size,
                                             &input_buf_position);
            key.search_cursor = 0;
            SDL_StartTextInput();
          } break;

          case SDLK_LEFT: {
            if (dir_count > 0) {
              vis.last_state = vis.current_state;
              vis.current_state = DIRECTORIES;
            }
          } break;

          case SDLK_RIGHT: {
            if (vis.stream_flag) {
              vis.last_state = vis.current_state;
              vis.current_state = PLAYBACK;
            }
          } break;

          case SDLK_UP: {
            nav_up(&key.file_cursor, &file_count);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.file_cursor, &file_count);
          } break;

          case SDLK_SPACE: {
            if (file_text_buffer) {
              KILL_PARTICLES(particle_buffer, particle_buffer_size);
              select_file(adc, seek_path(file_text_buffer, &key.file_cursor,
                                         file_contents));
            }
          } break;
          }

        } break;

        case DIRECTORIES: {
          switch (event.key.keysym.sym) {
          default:
            break;

          case SDLK_s: {
            vis.last_state = vis.current_state;
            vis.current_state = SEARCHING_DIRS;
            input_buf_position = 0;
            text_input_buffer[0] = '\0';
            search_text = destroy_search_text(search_text);
            search_text = create_search_text(text_input_buffer, &input_buf_size,
                                             &input_buf_position);
            key.search_cursor = 0;
            SDL_StartTextInput();
          } break;

          case SDLK_LEFT: {
            if (vis.stream_flag) {
              vis.last_state = vis.current_state;
              vis.current_state = PLAYBACK;
            }
          } break;

          case SDLK_RIGHT: {
            if (file_count > 0) {
              vis.last_state = vis.current_state;
              vis.current_state = SONGS;
            }
          } break;

          case SDLK_UP: {
            nav_up(&key.dir_cursor, &dir_count);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.dir_cursor, &dir_count);
          } break;

          case SDLK_SPACE: {
            if (dir_text_buffer) {
              fill_file_contents(
                  &file_contents, &file_text_buffer, &file_count,
                  seek_path(dir_text_buffer, &key.dir_cursor, dir_contents),
                  find_files);
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

    case SEARCHING_SONGS: {
      render_draw_search_text(search_text);
    } break;

    case SEARCHING_DIRS: {
      render_draw_search_text(search_text);
    } break;

    case DISPLAY_SEARCH: {
      render_draw_text(filtered_tb, &filter_count, &key.search_cursor);
    } break;

    case SONGS: {
      render_draw_text(file_text_buffer, &file_count, &key.file_cursor);
    } break;

    case DIRECTORIES: {
      render_draw_text(dir_text_buffer, &dir_count, &key.dir_cursor);
    } break;

    case PLAYBACK: {
      if (vis.dev) {
        switch (vis.next_song_flag) {
        case 1: {
          if (!vis.scrolling) {
            nav_down(&key.file_cursor, &file_count);
            KILL_PARTICLES(particle_buffer, particle_buffer_size);
            select_file(adc, seek_path(file_text_buffer, &key.file_cursor,
                                       file_contents));
          }
        } break;

        case 0: {
          int status = SDL_GetAudioDeviceStatus(vis.dev);
          if (status == SDL_AUDIO_PLAYING) {
            float tmp[M_BUF_SIZE];
            memcpy(tmp, f_buffers->fft_in, sizeof(float) * M_BUF_SIZE);
            hamming_window(tmp, f_data->hamming_values, f_buffers->windowed);
            iter_fft(f_buffers->windowed, f_buffers->out_raw, M_BUF_SIZE);
            extract_frequencies(f_buffers);
            freq_bin_algo(adc->SR, f_buffers->extracted);
            squash_to_log(f_buffers, f_data);
            visual_refine(f_buffers, f_data);
          }

          if (f_data->output_len > 0) {
            render_draw_music(f_buffers->smear, f_buffers->smoothed,
                              f_buffers->windowed, &f_data->output_len,
                              particle_buffer);
          }

        } break;
        }
      }
    } break;
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  file_contents = free_paths(file_contents, &file_count);
  dir_contents = free_paths(dir_contents, &dir_count);

  dir_text_buffer = free_text_buffer(dir_text_buffer, &dir_count);
  file_text_buffer = free_text_buffer(file_text_buffer, &file_count);

  if (particle_buffer) {
    free(particle_buffer);
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

int get_char_limit(int width) {
  const int sub_amount = width * 0.5;
  if (width < 100) {
    return 1;
  }
  return MIN(150, MAX(8, (width - sub_amount) / 10));
}

int get_title_limit(int height) {
  const int sub_amount = height * 0.75;
  return MIN(16, MAX(1, (height - sub_amount) / 16));
}

void *scp(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "SDL failed to create PTR! -> %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return ptr;
}

int scc(int code) {
  if (code < 0) {
    fprintf(stderr, "SDL code execution failed! -> %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  return code;
}
