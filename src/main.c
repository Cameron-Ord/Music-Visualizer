#include "main.h"
#include "SDL2/SDL_platform.h"
#include "filesystem.h"
#include "fontdef.h"
#include "utils.h"
#include <math.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))



#ifdef _WIN32
Paths* (*find_directories)(size_t *) = &win_find_directories;  
Paths* (*find_files)(size_t *, char *) = &win_find_files;  
#endif

#ifdef __linux__
Paths* (*find_directories)(size_t *) = &unix_find_directories;  
Paths* (*find_files)(size_t *, char *) = &unix_find_files;  
#endif


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

  const char *platform = SDL_GetPlatform();
  if(strcmp(platform, "Linux") != 0 && strcmp(platform, "Windows") != 0){
    fprintf(stderr, "Wrong platform\n");
    exit(EXIT_FAILURE);
  }

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

  FileSys files = {0};

  size_t dir_count = 0;
  size_t file_count = 0;

  printf("Finding directories..\n");
  Paths *dir_contents = find_directories(&dir_count);
  Paths* file_contents = NULL;

  if (!dir_contents) {
    fprintf(stderr, "Failed to read directories! -> EXIT\n");
    exit(EXIT_FAILURE);
  }

  files.dir_count = dir_count;
  files.file_count = file_count;

  size_t d_list_size = 1;
  size_t d_subbuf_size = rend.title_limit;

  TextBuffer *dir_text_list = create_directory_fonts(
      dir_contents, dir_count, &d_list_size, &d_subbuf_size);
  if (!dir_text_list) {
    fprintf(stderr, "Text creation failed! -> EXIT\n");
    exit(EXIT_FAILURE);
  }

  scc(SDL_SetRenderDrawBlendMode(rend.r, SDL_BLENDMODE_BLEND));
  SDL_EnableScreenSaver();

  const int ticks_per_frame = (1000.0 / 60);
  uint64_t frame_start;
  int frame_time;
  printf("Entering main loop..\n");

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();

    render_bg();
    render_clear();
    switch (vis.current_state) {
    default:
      break;

    case DIRECTORIES: {
      render_draw_text(&dir_text_list[key.dir_list_index], &key.dir_cursor,
                       &d_subbuf_size);
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
            {
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
            {
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
            {
              const Text* text_buf = dir_text_list[key.dir_list_index].buf;
              const char *search_key = text_buf[key.dir_cursor].name;
              
              file_contents = find_files(&file_count, find_char_str(search_key, dir_contents));
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

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  SDL_DestroyRenderer(rend.r);
  SDL_DestroyWindow(win.w);

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
