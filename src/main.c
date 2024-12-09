#include "main.h"
#include "audio.h"
#include "audiodefs.h"
#include "events.h"
#include "filesystem.h"
#include "font.h"
#include "renderer.h"
#include "table.h"
#include "utils.h"
#include <SDL2/SDL_timer.h>
#include <errno.h>
#include <stdint.h>

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

typedef enum { PLAYBACK = 1, TEXT = 0 } MODE_ENUM;

// Using catppuccin mocha as the default theme.
// https://github.com/catppuccin/catppuccin

SDL_Color primary = {203, 166, 247, 255};
SDL_Color secondary = {166, 227, 161, 255};
SDL_Color background = {30, 30, 46, 255};
SDL_Color secondary_bg = {49, 50, 68, 255};
SDL_Color text = {205, 214, 244, 255};

int FPS = 60;

static void replace_fonts(Table *t, SDL_Renderer *r, Font *f, const int w,
                          const SDL_Color *c_text, const SDL_Color *c_sec);
static int open_ttf_file(const char *fn, const char *home, Font *font);
static void window_resized(Window *w, const int font_size, int *char_limit);
static int valid_ptr(Paths *pbuf, TextBuffer *tbuf);
static void swap_font_ptrs(Table *table, const size_t key,
                           TextBuffer *old_buffer, TextBuffer *replace);
static RenderArgs make_args(const FFTData *d, const FFTBuffers *b);

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
    SDL_ERR_CALLBACK(SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (TTF_Init() != 0) {
    SDL_ERR_CALLBACK(SDL_GetError());
    exit(EXIT_FAILURE);
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

  fprintf(stdout, "home path -> %s\n", home);
  SDL_AudioSpec spec = {0};

  Visualizer vis = {.dev = 0,
                    .spec = &spec,
                    .quit = 0,
                    .target_frames = FPS,
                    .smearing = 6,
                    .smoothing = 8,
                    .home = home};

  Colors colors = {.primary = primary,
                   .secondary = secondary,
                   .background = background,
                   .secondary_bg = secondary_bg,
                   .text = text};

  Window w = {.w = NULL, .height = WIN_H, .width = WIN_W};
  Renderer r = {.r = NULL};

  w.w = SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
  if (!w.w) {
    SDL_ERR_CALLBACK(SDL_GetError());
    exit(EXIT_FAILURE);
  }
  r.r = SDL_CreateRenderer(
      w.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!r.r) {
    SDL_ERR_CALLBACK(SDL_GetError());
    SDL_DestroyWindow(w.w);
    exit(EXIT_FAILURE);
  }

  Font font = {.font = NULL, .char_limit = 0, .size = 16};

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

    SDL_Color *color_ptrs[] = {&colors.primary, &colors.secondary,
                               &colors.background, &colors.secondary_bg,
                               &colors.text};

    const size_t col_size = sizeof(color_ptrs) / sizeof(color_ptrs[0]);
    for (size_t i = 0; i < col_size; i++) {
      lua_getfield(L, -1, fields[i]);
      for (size_t j = 0; j < 4; j++) {

        /*
          int lua_gettable (lua_State *L, int index);

          Pushes onto the stack the value t[k], where t is the value at the
          given index and k is the value at the top of the stack.

          This function pops the key from the stack, pushing the resulting
          value in its place. As in Lua, this function may trigger a
          metamethod for the "index" event (see §2.4).
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

  if (!open_ttf_file("dogicapixel.ttf", home, &font)) {
    exit(EXIT_FAILURE);
  }

  font.char_limit = get_char_limit(w.width, font.size);

  AudioDataContainer adc = {0};
  FFTBuffers f_buffers = {0};
  FFTData f_data = {0};
  zero_fft(&f_buffers, &f_data);
  adc.buffer = NULL;

  adc.next = &f_buffers;
  adc.dev_ptr = &vis.dev;
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
  Paths *p = fs_search(home);
  TextBuffer *t =
      create_fonts(p, r.r, &font, w.width, &colors.text, &colors.secondary);

  table_set_paths(&table, current_node, p);
  table_set_text(&table, current_node, t);

  memset(f_data.hamming_values, 0, sizeof(float) * M_BUF_SIZE);
  calculate_window(f_data.hamming_values);

  SDL_SetRenderDrawBlendMode(r.r, SDL_BLENDMODE_BLEND);
  SDL_EnableScreenSaver();
  SDL_ShowWindow(w.w);

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  uint64_t song_end_time = SDL_GetTicks64();
  SDL_StopTextInput();

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();

    render_bg(&colors.background, r.r);
    render_clear(r.r);

    if (adc.buffer && adc.position < adc.length) {
      if (get_status(&vis.dev) == SDL_AUDIO_PLAYING) {
        float tmp[M_BUF_SIZE];
        memcpy(tmp, f_buffers.fft_in, sizeof(float) * M_BUF_SIZE);

        iter_fft(tmp, f_data.hamming_values, f_buffers.out_raw, M_BUF_SIZE);
        squash_to_log(&f_buffers, &f_data);
        linear_mapping(&f_buffers, &f_data, vis.smearing, vis.smoothing,
                       vis.target_frames);
      }
    } else if (adc.buffer && adc.position >= adc.length) {
      if (get_status(&vis.dev) == SDL_AUDIO_PLAYING ||
          get_status(&vis.dev) == SDL_AUDIO_PAUSED) {
        // This will block if it needs to.
        close_device(vis.dev);
        song_end_time = SDL_GetTicks64();
      }

      if (get_status(&vis.dev) == SDL_AUDIO_STOPPED &&
          SDL_GetTicks64() - song_end_time > 5) {
        int file_valid = 0;
        int read_valid = 0;

        TextBuffer *t = search_table(&table, playing_node)->tbuf;
        Paths *p = search_table(&table, playing_node)->pbuf;
        if (valid_ptr(p, t)) {
          playing_cursor = auto_nav_down(playing_cursor, t->size);

          const char *item_path = find_pathstr(t[playing_cursor].text->name, p);
          const int item_type = find_type(t[playing_cursor].text->name, p);
          if (item_type == TYPE_FILE) {
            file_valid = 1;
          }

          if (file_valid && read_audio_file(item_path, &adc)) {
            read_valid = 1;
          }

          if (read_valid) {
            set_spec(&adc, vis.spec);
          }

          vis.dev = open_device(vis.spec);
          if (vis.dev) {
            resume_device(vis.dev);
          }
        }
      }
    }

    // delegate these

    switch (mode) {
    default:
      break;

    case TEXT: {
      Node *n = search_table(&table, current_node);
      if (valid_ptr(n->pbuf, n->tbuf)) {
        MaxValues m = determine_max(n->tbuf, w.height);
        render_draw_subg_outline(r.r, w.width, w.height, &colors.secondary, &m);
        render_draw_subbg(r.r, w.width, w.height, &colors.secondary_bg, &m);

        render_draw_text(r.r, n->tbuf, w.height, w.width, &m);
      }
    } break;

    case PLAYBACK: {
      RenderArgs args = make_args(&f_data, &f_buffers);
      render_seek_bar(&adc.position, &adc.length, w.width, &colors.primary,
                      r.r);
      render_draw_music(&args, w.width, w.height, r.r, &colors.primary,
                        &colors.secondary_bg);
    } break;
    }

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
          window_resized(&w, font.size, &font.char_limit);
          replace_fonts(&table, r.r, &font, w.width, &colors.text,
                        &colors.secondary);
        } break;

        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          window_resized(&w, font.size, &font.char_limit);
          replace_fonts(&table, r.r, &font, w.width, &colors.text,
                        &colors.secondary);
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
                close_device(vis.dev);

                int valid = 0;
                if (read_audio_file(item_path, &adc)) {
                  valid = 1;
                }

                if (valid) {
                  set_spec(&adc, vis.spec);
                }

                vis.dev = open_device(vis.spec);
                if (vis.dev) {
                  playing_node = current_node;
                  playing_cursor = t->cursor;
                  mode = PLAYBACK;
                  resume_device(vis.dev);
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
                  Paths *p = search_table(&table, i)->pbuf;
                  TextBuffer *t = create_fonts(p, r.r, &font, w.width,
                                               &colors.text, &colors.secondary);
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
              TextBuffer *t = search_table(&table, current_node)->tbuf;
              if (t) {
                t->cursor = nav_down(t->cursor, t->size);
                t->start = mv_start_pos(1, t->start, t->size);
              }
            }
          } break;

          case SDLK_UP: {
            if (keymod & KMOD_SHIFT) {
              mode = TEXT;
            } else {
              TextBuffer *t = search_table(&table, current_node)->tbuf;
              if (t) {
                t->cursor = nav_up(t->cursor, t->size);
                t->start = mv_start_pos(-1, t->start, t->size);
              }
            }
          } break;
          }
        } break;

        case PLAYBACK: {
          switch (keysym) {
          default:
            break;

          case SDLK_p: {
            if (get_status(&vis.dev) == SDL_AUDIO_PAUSED) {
              resume_device(vis.dev);
            } else if (get_status(&vis.dev) == SDL_AUDIO_PLAYING) {
              pause_device(vis.dev);
            }
          } break;

          case SDLK_DOWN: {
            if (keymod & KMOD_SHIFT) {
              mode = PLAYBACK;
            }
          } break;

          case SDLK_UP: {
            if (keymod & KMOD_SHIFT) {
              mode = TEXT;
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

    render_present(r.r);
  }

  close_device(vis.dev);

  if (adc.buffer) {
    free(adc.buffer);
  }

  if (r.r) {
    SDL_DestroyRenderer(r.r);
  }

  if (w.w) {
    SDL_DestroyWindow(w.w);
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}

static int open_ttf_file(const char *fn, const char *home, Font *font) {
  const size_t path_size =
      get_length(3, strlen(fn), strlen(home), strlen(ASSETS_DIR));

  char *path_buffer = malloc(path_size + 1);
  if (!path_buffer) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
    return 0;
  }

  const size_t written =
      snprintf(path_buffer, path_size + 1, "%s%s%s", home, ASSETS_DIR, fn);
  if (written <= 0) {
    fprintf(stderr, "snprintf failed! -> %s\n", strerror(errno));
    return 0;
  }

  font->font = TTF_OpenFont(path_buffer, font->size);
  free(path_buffer);
  if (!font->font) {
    SDL_ERR_CALLBACK(SDL_GetError());
    return 0;
  }

  return 1;
}

static void replace_fonts(Table *t, SDL_Renderer *r, Font *f, const int w,
                          const SDL_Color *c_text, const SDL_Color *c_sec) {
  for (int i = 0; i < MAX_NODES; i++) {
    Node *n = search_table(t, i);
    if (n) {
      TextBuffer *txt = create_fonts(n->pbuf, r, f, w, c_text, c_sec);
      swap_font_ptrs(t, i, n->tbuf, txt);
    }
  }
}

static RenderArgs make_args(const FFTData *d, const FFTBuffers *b) {
  RenderArgs a;
  //&output_len is not a dangling ptr since its from outside this
  // scope, and not limited to it.
  a.smear = b->smear, a.length = &d->output_len, a.smooth = b->smoothed;
  return a;
}

static void window_resized(Window *w, const int font_size, int *char_limit) {
  SDL_GetWindowSize(w->w, &w->width, &w->height);
  *char_limit = get_char_limit(w->width, font_size);
}

static int valid_ptr(Paths *p, TextBuffer *t) {
  if ((p && t) && (p->is_valid && t->is_valid)) {
    return 1;
  }

  return 0;
}

static void swap_font_ptrs(Table *table, const size_t key,
                           TextBuffer *old_buffer, TextBuffer *replace) {
  table_set_text(table, key, replace);

  if (old_buffer) {
    for (size_t i = 0; i < old_buffer->size; i++) {
      Text *invalidated = old_buffer[i].text;
      char *invalid_name = invalidated->name;
      SDL_Texture **invalid_tex = invalidated->tex;

      if (invalid_name) {
        free(invalid_name);
      }

      if (invalid_tex[0]) {
        SDL_DestroyTexture(invalid_tex[0]);
      }

      if (invalid_tex[1]) {
        SDL_DestroyTexture(invalid_tex[1]);
      }

      free(invalidated);
    }

    free(old_buffer);
  }
}
