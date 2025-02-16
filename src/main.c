#include "../inc/main.h"
#include "../inc/audio.h"
#include "../inc/events.h"
#include "../inc/font.h"
#include "../inc/renderer.h"
#include "../inc/sys.h"
#include "../inc/table.h"
#include "../inc/window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __linux__
#include <unistd.h>
#define HOME "HOME"
#define ASSETS_DIR "/.local/share/MVis/"
Paths *(*fs_search)(const char *) = &unix_fs_search;
#endif

typedef enum { PLAYBACK = 1, TEXT = 0 } MODE_ENUM;

// Using catppuccin mocha as the default theme.
// https://github.com/catppuccin/catppuccin

const SDL_Color primary = {203, 166, 247, 255};
const SDL_Color secondary = {166, 227, 161, 255};
const SDL_Color background = {30, 30, 46, 255};
const SDL_Color secondary_bg = {49, 50, 68, 255};
const SDL_Color text = {205, 214, 244, 255};

const SDL_Color *_text(void) { return &text; }
const SDL_Color *_sec(void) { return &secondary; }

const int FPS = 60;

Visualizer vis = {.quit = 0,
                  .target_frames = FPS,
                  .smearing = 3,
                  .smoothing = 8,
                  .home = NULL};

void sdl_err(const char *msg) { fprintf(stderr, "%s\n", msg); }

void errno_string(const char *func, const char *msg) {
  fprintf(stderr, "%s failed! -> %s\n", func, msg);
}

void set_cursor(const int set, int *target) {
  if (!target) {
    return;
  }
  *target = set;
}

static int autoplay(Paths *current_paths) {
  set_cursor(auto_nav_down(current_paths->cursor, current_paths->size),
             &current_paths->cursor);

  const char *pathstr = current_paths[current_paths->cursor].path.path;
  const int entry_type = current_paths[current_paths->cursor].type;

  switch (entry_type) {
  default:
    return 0;

  case TYPE_DIRECTORY: {
    return 0;
  } break;

  case TYPE_FILE: {
    switch (_file_read(pathstr)) {
    default:
      return 0;

    case 1: {
      return _start_device();
    } break;
    }
  } break;
  }

  return 0;
}

int main(int argc, char **argv) {
#ifdef __linux__
  // Root processes have an effective user ID of 0
  if (geteuid() == 0) {
    fprintf(stderr, "Please run as a user and not as root!\n");
    return 1;
  }
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
    sdl_err(SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (TTF_Init() != 0) {
    sdl_err(SDL_GetError());
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  const char *platform = SDL_GetPlatform();
  if (strcmp(platform, "Linux") != 0) {
    fprintf(stderr, "Wrong platform\n");
    exit(EXIT_FAILURE);
  }

  char *home = getenv(HOME);
  if (!home) {
    fprintf(stderr, "Failed to get home environment! -> %s\n", strerror(errno));
    return 1;
  }

  fprintf(stdout, "env -> %s\n", home);
  fprintf(stdout, "FPS TARGET -> %d\n", vis.target_frames);

  if (!create_window()) {
    exit(EXIT_FAILURE);
  }

  if (!create_renderer()) {
    exit(EXIT_FAILURE);
  }

  if (!_open_ttf_file("/.local/share/mvis/dogicapixel.ttf", home)) {
    exit(EXIT_FAILURE);
  }

  if (!_fill_text_atlas()) {
    exit(EXIT_FAILURE);
  }

  // Set hamming window values
  calculate_window();

  Table table;
  for (size_t i = 0; i < MAX_NODES; i++) {
    table.node_buffer[i] = NULL;
    if (!create_node(&table, i)) {
      exit(EXIT_FAILURE);
    }
  }

  table_set_paths(&table, fs_search(home));
  Paths *current_paths = search_table(&table, get_current_index())->paths;

  SDL_EnableScreenSaver();

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  SDL_ShowWindow(get_window()->w);

  int mode = TEXT;

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();
    render_bg(&background);
    render_clear();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      default:
        break;

      case SDL_KEYDOWN: {
        const int keysym = e.key.keysym.sym;
        const int modkey = e.key.keysym.mod;

        switch (keysym) {
        default:
          break;

        case SDLK_p: {
          switch (_get_status()) {
          default:
            break;

          case SDL_AUDIO_PAUSED: {
            _resume();
          } break;

          case SDL_AUDIO_PLAYING: {
            _pause();
          } break;
          }
        } break;

        case SDLK_LEFT: {
          switch (mode) {
          case TEXT: {
            set_current_index(nav_up(get_current_index(), MAX_NODES));
            current_paths = search_table(&table, get_current_index())->paths;
          } break;

          case PLAYBACK: {
          } break;
          }
        } break;

        case SDLK_RIGHT: {
          switch (mode) {
          case TEXT: {
            const char *pathstr =
                current_paths[current_paths->cursor].path.path;
            const int entry_type = current_paths[current_paths->cursor].type;

            switch (entry_type) {
            default:
              break;
            case TYPE_FILE: {
              switch (_file_read(pathstr)) {
              default:
                break;

              case 1: {
                if (_start_device()) {
                  mode = PLAYBACK;
                }
              } break;
              }
            } break;

            case TYPE_DIRECTORY: {
              set_last_index(get_current_index());
              set_current_index(nav_down(get_current_index(), MAX_NODES));
              switch (table_set_paths(&table, fs_search(pathstr))) {
              case 0: {
                set_current_index(get_last_index());
              } break;

              case 1: {
                current_paths =
                    search_table(&table, get_current_index())->paths;
              } break;
              }
            } break;
            }
          } break;
          case PLAYBACK: {
          } break;
          }
        } break;

        case SDLK_DOWN: {
          if (modkey & KMOD_SHIFT) {
            mode = !mode;
          } else {
            switch (mode) {
            case TEXT: {
              set_cursor(nav_down(current_paths->cursor, current_paths->size),
                         &current_paths->cursor);
            } break;
            case PLAYBACK: {
            } break;
            }
          }
        } break;

        case SDLK_UP: {
          if (modkey & KMOD_SHIFT) {
            mode = !mode;
          } else {
            switch (mode) {
            case TEXT: {
              set_cursor(nav_up(current_paths->cursor, current_paths->size),
                         &current_paths->cursor);
            } break;
            case PLAYBACK: {
            } break;
            }
          }

        } break;
        }
      } break;

      case SDL_QUIT: {
        vis.quit = 1;
      } break;

      case SDL_WINDOWEVENT: {
        switch (e.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          win_push_update();

        } break;
        case SDL_WINDOWEVENT_RESIZED: {
          win_push_update();
        } break;
        }
      } break;
      }
    }

    switch (mode) {
    case PLAYBACK: {
      switch (_get_status()) {
      case SDL_AUDIO_PAUSED: {
        const int gate = get_ad()->position < get_ad()->length && get_ad();
        switch (gate) {
        default:
          break;
        case 0: {
          if (!autoplay(current_paths)) {
            mode = TEXT;
          }
        } break;

        case 1: {
          _do_fft(&vis.smearing, &vis.smoothing, &vis.target_frames);
          render_seek_bar(get_window()->width, &primary, get_renderer()->r);
          SDL_Color p_alpha_mod = {primary.r, primary.g, primary.b, 125};
          SDL_Color s_alpha_mod = {secondary_bg.r, secondary_bg.g,
                                   secondary_bg.b, 125};
          render_draw_music(get_window()->width, get_window()->height,
                            get_renderer()->r, &p_alpha_mod, &s_alpha_mod);
        } break;
        }
      } break;

      case SDL_AUDIO_PLAYING: {
        const int gate = get_ad()->position < get_ad()->length && get_ad();
        switch (gate) {
        default:
          break;
        case 1: {
          _do_fft(&vis.smearing, &vis.smoothing, &vis.target_frames);
          render_seek_bar(get_window()->width, &primary, get_renderer()->r);
          render_draw_music(get_window()->width, get_window()->height,
                            get_renderer()->r, &primary, &secondary_bg);

        } break;
        }

      } break;

      case SDL_AUDIO_STOPPED: {
        mode = TEXT;
      } break;
      }
    } break;

    case TEXT: {
      render_node_text(current_paths);
    } break;
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}
