#include "main.h"
#include "filesystem.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

Window win;
Renderer rend;
Font font;
Visualizer vis;
Events key;

SDL_Color primary = {166, 172, 205, 255};
SDL_Color secondary = {68, 66, 103, 255};
SDL_Color background = {41, 45, 62, 255};
SDL_Color text = {103, 110, 149, 255};
SDL_Color text_bg = {113, 124, 180, 255};

int main(int argc, char **argv) {
  scc(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
  scc(TTF_Init());

  fprintf(stdout, "Starting..\n");

  key.dir_cursor = 0;
  key.song_cursor = 0;
  key.song_list_index = 0;
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

  win.w = (SDL_Window *)scp(SDL_CreateWindow(
      "Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W,
      WIN_H, SDL_WINDOW_RESIZABLE));
  rend.r = (SDL_Renderer *)scp(SDL_CreateRenderer(
      win.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

  win.height = WIN_H;
  win.width = WIN_W;

  rend.song_locn = 0;
  rend.dir_locn = 0;
  font.font = NULL;
  font.char_limit = 0;
  font.size = 16;

  font.font = (TTF_Font *)scp(TTF_OpenFont(FONT_PATH, font.size));

  rend.title_limit = get_title_limit(win.height);
  font.char_limit = get_char_limit(win.width);

  FileSys files = {0};

  size_t dir_count = 0;
  size_t file_count = 0;

#ifdef _WIN32
  char **dir_buf = win_find_directories(&dir_count);
#else
  dir_buf = unix_find_directories();
#endif

  files.dir_titles = dir_buf;
  files.file_titles = NULL;
  files.dir_count = dir_count;
  files.file_count = file_count;

  size_t dtext_buf_size = 1;
  size_t dsub_buf_size = rend.title_limit;

  TextBuffer **dir_text_buf = create_directory_fonts(
      dir_buf, dir_count, &dtext_buf_size, &dsub_buf_size);
  if (!dir_text_buf) {
    fprintf(stderr, "Could not allocate text buffer!\n");
  }

  scc(SDL_SetRenderDrawBlendMode(rend.r, SDL_BLENDMODE_BLEND));
  SDL_EnableScreenSaver();

  const int ticks_per_frame = (1000.0 / 60);
  uint64_t frame_start;
  int frame_time;

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();

    render_bg();
    render_clear();
    switch (vis.current_state) {
    default:
      break;

    case DIRECTORIES: {
      // render_draw_text(dir_text_buf[key.dir_list_index], &key.dir_cursor,
      // &dsub_buf_size);
    } break;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      default:
        break;

      case SDL_KEYDOWN: {
        switch (vis.current_state) {
        default:
          break;

        case DIRECTORIES: {
          switch (event.key.keysym.sym) {
          default:
            break;
          case SDLK_UP: {
            nav_up(&key.dir_cursor, &key.dir_list_index, dsub_buf_size);
          } break;

          case SDLK_DOWN: {
            nav_down(&key.dir_cursor, &key.dir_list_index, dsub_buf_size);
          }
          }
        }
        }
      } break;

      case SDL_QUIT: {
        vis.quit = true;

      } break;
      }
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

int get_char_limit(int width) { return fmin(175, fmax(8, (width - 200) / 12)); }

int get_title_limit(int height) {
  return fmin(24, fmax(1, (height - 100) / 32));
}

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
