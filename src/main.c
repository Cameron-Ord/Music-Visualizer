#include "main.h"
#include "audio.h"
#include "audiodefs.h"
#include "filesystem.h"
#include "fontdef.h"
#include "table.h"
#include "utils.h"

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_video.h>
#include <errno.h>

#ifdef LUA_FLAG
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#endif

#ifdef _WIN32
#define HOME "USERPROFILE"
#define ASSETS_DIR "\\Documents\\share\\MVis\\"
Paths *(*fs_search)(const char *) = &win_fs_search;
#endif

#ifdef __linux__
#define HOME "HOME"
#define ASSETS_DIR "/.local/share/MVis/"
Paths *(*fs_search)(const char *) = &unix_fs_search;
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Window win;
Renderer rend;
Font font;
Visualizer vis;

typedef enum { PLAYBACK = 1, TEXT = 0 } MODE_ENUM;

//Using catppuccin mocha as the default theme.
//https://github.com/catppuccin/catppuccin

SDL_Color primary = {203, 166, 247, 255};
SDL_Color secondary = {137, 180, 250, 255}; 
SDL_Color background = {30, 30, 46, 255};  
SDL_Color secondary_bg = {49, 50, 68, 255};
SDL_Color text = {205, 214, 244, 255};      

int FPS = 60;

static void replace_fonts(Table *t);

size_t hash(size_t i) { return i % MAX_NODES; }

int create_node(Table *t, size_t i) {
  Node *n = malloc(sizeof(Node));
  if (!n) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
    return 0;
  }

  n->key = hash(i);
  n->pbuf = NULL;
  n->tbuf = NULL;
  n->next = t->node_buffer[n->key];
  t->node_buffer[n->key] = n;

  return 1;
}

Node *search_table(Table *t, size_t i) {
  Node *n = t->node_buffer[hash(i)];
  while (n != NULL) {
    if (n->key == hash(i)) {
      return n;
    }
    n = n->next;
  }
  return NULL;
}

void table_set_text(Table *t, size_t i, TextBuffer *tbuf) {
  Node *n = search_table(t, i);
  if (!n) {
    return;
  }

  if (!tbuf) {
    n->tbuf = NULL;
    // We do the check on the validity, then free or assign memory as per it's
    // value. Easier to manage this way.
  } else if (tbuf && !tbuf->is_valid) {
    // returns null
    n->tbuf = free_text_buffer(tbuf, &tbuf->size);
  } else {
    n->tbuf = tbuf;
  }
}

void table_set_paths(Table *t, size_t i, Paths *pbuf) {
  Node *n = search_table(t, i);
  if (!n) {
    return;
  }

  if (!pbuf) {
    n->pbuf = NULL;
    // We do the check on the validity, then free or assign memory as per it's
    // value. Easier to manage this way.
  } else if (pbuf && !pbuf->is_valid) {
    // returns null
    n->pbuf = free_paths(pbuf, &pbuf->size);
  } else {
    n->pbuf = pbuf;
  }
}

int main(int argc, char **argv) {
  scc(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
  scc(TTF_Init());

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

  fprintf(stdout, "home path -> %s\n", home);

  // Assign default values
  vis.home = home;
  vis.smearing = 6;
  vis.smoothing = 8;
  vis.target_frames = FPS;
  vis.awaiting = 0;
  vis.quit = false;
  vis.primary = primary;
  vis.secondary = secondary;
  vis.background = background;
  vis.secondary_bg = secondary_bg;
  vis.text = text;

  win.w = NULL;
  rend.r = NULL;

  win.w = scp(SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN));

  rend.r = scp(SDL_CreateRenderer(
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

    const char *fields[] = {"primary", "secondary", "background",
                            "secondary_bg", "text"};

    SDL_Color *color_ptrs[] = {&vis.primary, &vis.secondary, &vis.background,
                               &vis.secondary_bg, &vis.text};

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

  if (!open_ttf_file("dogicapixel.ttf")) {
    exit(EXIT_FAILURE);
  }

  font.char_limit = get_char_limit(win.width);

  AudioDataContainer adc;
  FFTBuffers f_buffers;
  FFTData f_data;
  zero_values(&adc);
  zero_fft(&f_buffers, &f_data);
  adc.buffer = NULL;

  adc.next = &f_buffers;
  f_buffers.next = &f_data;
  f_data.next = &adc;

  size_t current_node = 0;
  size_t playing_node = 0;
  size_t playing_cursor = 0;

  Table table;
  for (size_t i = 0; i < MAX_NODES; i++) {
    table.node_buffer[i] = NULL;
    if (!create_node(&table, i)) {
      exit(EXIT_FAILURE);
    }
  }

  int mode = TEXT;

  table_set_paths(&table, current_node, fs_search(home));
  table_set_text(&table, current_node,
                 create_fonts(search_table(&table, current_node)->pbuf));

  memset(f_data.hamming_values, 0, sizeof(float) * M_BUF_SIZE);
  calculate_window(f_data.hamming_values);

  scc(SDL_SetRenderDrawBlendMode(rend.r, SDL_BLENDMODE_BLEND));
  SDL_EnableScreenSaver();
  SDL_ShowWindow(win.w);

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  SDL_StopTextInput();

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
        default:
          break;
        case SDL_WINDOWEVENT_RESIZED: {
          window_resized();
          replace_fonts(&table);
        } break;

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          window_resized();
          replace_fonts(&table);
        } break;
        }
      } break;

      case SDL_KEYDOWN: {
        int keysym = event.key.keysym.sym;
        int keymod = event.key.keysym.mod;
        switch (mode) {
        default:
          break;

        case TEXT: {
          switch (keysym) {
          default:
            break;

          case SDLK_LEFT: {
            int i = node_index("--", current_node, MAX_NODES);
            Node *n = search_table(&table, i);
            if (n->pbuf && n->tbuf) {
              Text *t = n->tbuf[n->tbuf->cursor].text;
              if (find_pathstr(t->name, n->pbuf)) {
                current_node = i;
              } else {
                current_node = 0;
              }
            } else {
              current_node = 0;
            }
          } break;

          case SDLK_RIGHT: {
            TextBuffer *t = search_table(&table, current_node)->tbuf;
            Paths *p = search_table(&table, current_node)->pbuf;

            if (valid_ptr(p, t)) {
              const char *item_path = find_pathstr(t[t->cursor].text->name, p);
              const int item_type = find_type(t[t->cursor].text->name, p);

              switch (item_type) {
              default:
                break;

              case TYPE_FILE: {
                int valid = 0;
                if (read_audio_file(item_path, &adc)) {
                  valid = 1;
                }

                if (valid) {
                  load_song(&adc);
                  playing_node = current_node;
                  playing_cursor = t->cursor;
                  mode = PLAYBACK;
                }

              } break;

              case TYPE_DIRECTORY: {
                int i = node_index("++", current_node, MAX_NODES);
                // Dont want to overwrite the entry point node. Generally this
                // shouldn't happen, but the protection remains.
                if (i != 0) {
                  // Need to free existing memory beforehand (if it exists)
                  TextBuffer *old_tb = search_table(&table, i)->tbuf;
                  Paths *old_paths = search_table(&table, i)->pbuf;
                  old_tb = free_text_buffer(old_tb, &old_tb->size);
                  old_paths = free_paths(old_paths, &old_paths->size);

                  table_set_paths(&table, i, fs_search(item_path));
                  TextBuffer *t = create_fonts(search_table(&table, i)->pbuf);
                  table_set_text(&table, i, t);

                  if (search_table(&table, i)->pbuf &&
                      search_table(&table, i)->tbuf) {
                    current_node = i;
                  }
                }
              } break;
              }
            }
          } break;

          case SDLK_DOWN: {
            if (keymod & KMOD_SHIFT) {
              mode = PLAYBACK;
            } else {
              nav_down(search_table(&table, current_node)->tbuf);
            }
          } break;

          case SDLK_UP: {
            if (keymod & KMOD_SHIFT) {
              mode = TEXT;
            } else {
              nav_up(search_table(&table, current_node)->tbuf);
            }
          } break;
          }
        } break;

        case PLAYBACK: {
          switch (keysym) {
          default:
            break;

          case SDLK_DOWN: {
            if (keymod & KMOD_SHIFT) {
              mode = PLAYBACK;
            } else {
              nav_down(search_table(&table, current_node)->tbuf);
            }
          } break;

          case SDLK_UP: {
            if (keymod & KMOD_SHIFT) {
              mode = TEXT;
            } else {
              nav_up(search_table(&table, current_node)->tbuf);
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

    switch (mode) {
    default:
      break;

    case TEXT: {
      Node *n = search_table(&table, current_node);
      if (valid_ptr(n->pbuf, n->tbuf)) {
        render_draw_text(n->tbuf);
      }
    } break;

    case PLAYBACK: {
      if (adc.buffer && adc.position < adc.length) {
        float tmp[M_BUF_SIZE];
        memcpy(tmp, f_buffers.fft_in, sizeof(float) * M_BUF_SIZE);
        hamming_window(tmp, f_data.hamming_values, f_buffers.windowed);
        iter_fft(f_buffers.windowed, f_buffers.out_raw, M_BUF_SIZE);
        extract_frequencies(&f_buffers);
        filter(adc.SR, f_buffers.extracted);
        squash_to_log(&f_buffers, &f_data);
        linear_mapping(&f_buffers, &f_data);
      } else if (adc.buffer && adc.position >= adc.length) {
        int file_valid = 0;
        int read_valid = 0;

        TextBuffer *t = search_table(&table, playing_node)->tbuf;
        Paths *p = search_table(&table, playing_node)->pbuf;
        if (valid_ptr(p, t)) {

          auto_play_nav(t->size, &playing_cursor);

          const char *item_path = find_pathstr(t[playing_cursor].text->name, p);
          const int item_type = find_type(t[playing_cursor].text->name, p);

          if (item_type == TYPE_FILE) {
            file_valid = 1;
          }

          if (file_valid && read_audio_file(item_path, &adc)) {
            read_valid = 1;
          }

          if (read_valid) {
            load_song(&adc);
            mode = PLAYBACK;
          }
        }
      } else {
        mode = TEXT;
      }

      RenderArgs args = {.smear = f_buffers.smear,
                         .smooth = f_buffers.smoothed,
                         .length = &f_data.output_len};

      if (f_data.output_len > 0) {
        render_seek_bar(&adc.position, &adc.length);
        render_draw_music(&args);
      }
    } break;
    }

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    render_present();
  }

  if (adc.buffer) {
    free(adc.buffer);
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

int open_ttf_file(const char *fn) {
  const size_t path_size =
      get_length(3, strlen(fn), strlen(vis.home), strlen(ASSETS_DIR));

  char *path_buffer = malloc(path_size + 1);
  if (!path_buffer) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
    return 0;
  }

  const size_t written =
      snprintf(path_buffer, path_size + 1, "%s%s%s", vis.home, ASSETS_DIR, fn);
  if (written <= 0) {
    fprintf(stderr, "snprintf failed! -> %s\n", strerror(errno));
    return 0;
  }

  font.font = scp(TTF_OpenFont(path_buffer, FONT_SIZE));
  free(path_buffer);

  return 1;
}

static void replace_fonts(Table *t) {
  for (int i = 0; i < MAX_NODES; i++) {
    Node *n = search_table(t, i);
    if (n) {
      swap_font_ptrs(t, i, n->tbuf, create_fonts(n->pbuf));
    }
  }
}
