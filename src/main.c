#include "../inc/main.h"
#include "../inc/audio.h"
#include "../inc/events.h"
#include "../inc/font.h"
#include "../inc/renderer.h"
#include "../inc/sys.h"
#include "../inc/table.h"
#include "../inc/window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

  Table table;
  for (size_t i = 0; i < MAX_NODES; i++) {
    table.node_buffer[i] = NULL;
    if (!create_node(&table, i)) {
      exit(EXIT_FAILURE);
    }
  }

  table_set_paths(&table, fs_search(home));
  Paths *current_paths = search_table(&table, get_current_index())->paths;
  int node_index_changed = 0;

  SDL_EnableScreenSaver();

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  SDL_ShowWindow(get_window()->w);

  int mode = PLAYBACK;

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
        switch (e.key.keysym.sym) {
        default:
          break;

        case SDLK_LEFT: {
          set_current_index(nav_up(get_current_index(), MAX_NODES));
          current_paths = search_table(&table, get_current_index())->paths;
        } break;

        case SDLK_RIGHT: {
          const char *pathstr = current_paths[current_paths->cursor].path.path;
          const int entry_type = current_paths[current_paths->cursor].type;

          switch (entry_type) {
          default:
            break;
          case TYPE_FILE: {
            _file_read(pathstr);
            _start_device();
          } break;

          case TYPE_DIRECTORY: {
            set_current_index(nav_down(get_current_index(), MAX_NODES));
            table_set_paths(&table, fs_search(pathstr));
            current_paths = search_table(&table, get_current_index())->paths;
          } break;
          }
        } break;

        case SDLK_DOWN: {
          current_paths->cursor =
              nav_down(current_paths->cursor, current_paths->size);
        } break;

        case SDLK_UP: {
          current_paths->cursor =
              nav_up(current_paths->cursor, current_paths->size);
        } break;
        }

      } break;

      case SDL_QUIT: {
        vis.quit = 1;
      } break;
      }
    }

    render_node_text(current_paths);

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
