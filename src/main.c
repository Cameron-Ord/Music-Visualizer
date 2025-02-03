#include "../inc/main.h"
#include "../inc/audio.h"
#include "../inc/audiodefs.h"
#include "../inc/events.h"
#include "../inc/filesystem.h"
#include "../inc/font.h"
#include "../inc/fontdef.h"
#include "../inc/renderer.h"
#include "../inc/table.h"
#include "../inc/utils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#include <unistd.h>
#define HOME "HOME"
#define ASSETS_DIR "/.local/share/MVis/"
Paths *(*fs_search)(const char *) = &unix_fs_search;
#endif

typedef enum { PLAYBACK = 1, TEXT = 0 } MODE_ENUM;

// Using catppuccin mocha as the default theme.
// https://github.com/catppuccin/catppuccin

SDL_Color primary = {203, 166, 247, 255};
SDL_Color secondary = {166, 227, 161, 255};
SDL_Color background = {30, 30, 46, 255};
SDL_Color secondary_bg = {49, 50, 68, 255};
SDL_Color text = {205, 214, 244, 255};

int FPS = 60;

typedef struct {
  const char *path;
  Table *tbl;
  SDL_Renderer *r;
  Font *f;
  const int w;
  SDL_Color *text;
  SDL_Color *sec;
} TypeDirArgs;

static void destroy_buffers(Table *t);
static void replace_fonts(Table *t, SDL_Renderer *r, Font *f, const int w,
                          const SDL_Color *c_text, const SDL_Color *c_sec);
static int open_ttf_file(const char *fn, const char *home, Font *font);
static void window_resized(Window *w, const int font_size, int *char_limit);
static int valid_ptr(Paths *pbuf, TextBuffer *tbuf);
static void swap_font_ptrs(Table *table, const size_t key,
                           TextBuffer *old_buffer, TextBuffer *replace);
static RenderArgs make_args(const FFTData *d, const FFTBuffers *b);
static void do_fft(FFTBuffers *b, FFTData *d, const Visualizer *v);
static void autoplay(const size_t *playing_node, size_t *playing_cursor,
                     Table *tbl, Visualizer *v, AudioDataContainer *adc);
static void move_up(TextBuffer *t);
static void move_down(TextBuffer *t, const int h);
static int handle_type_file(const char *path, AudioDataContainer *adc,
                            Visualizer *v);
static int handle_type_dir(const int i, TypeDirArgs *args);
static int go_back(const int i, Table *tbl);

int main(int argc, char **argv) {
#ifdef __linux__
  // Root processes have an effective user ID of 0
  if (geteuid() == 0) {
    fprintf(stderr, "Please run as a user and not as root!\n");
    return 1;
  }
#endif

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
  // Use software fallback flag
  r.r = SDL_CreateRenderer(
      w.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!r.r) {
    SDL_ERR_CALLBACK(SDL_GetError());
    SDL_DestroyWindow(w.w);
    exit(EXIT_FAILURE);
  }

  Font font = {.font = NULL, .char_limit = 0, .size = 16};
  fprintf(stdout, "FPS TARGET -> %d\n", vis.target_frames);

  if (!open_ttf_file("dogicapixel.ttf", home, &font)) {
    exit(EXIT_FAILURE);
  }

  font.char_limit = get_char_limit(w.width, font.size);

  AudioData ad = {0};
  FFTBuffers f_buffers = {0};
  FFTData f_data = {0};

  AudioDataContainer adc = {.buffer = NULL,
                            .ad = &ad,
                            .fftbuff = &f_buffers,
                            .fftdata = &f_data,
                            &vis.dev};

  zero_fft(&f_buffers, &f_data);

  NodeIdx node_idx = {.p_node = 0, .cur_node = 0, .node_curs = 0};

  Table table;
  for (size_t i = 0; i < MAX_NODES; i++) {
    table.node_buffer[i] = NULL;
    if (!create_node(&table, i)) {
      exit(EXIT_FAILURE);
    }
  }

  int mode = TEXT;
  Paths *first_paths = fs_search(home);
  TextBuffer *first_text = create_fonts(first_paths, r.r, &font, w.width,
                                        &colors.text, &colors.secondary);

  table_set_paths(&table, node_idx.cur_node, first_paths);
  table_set_text(&table, node_idx.cur_node, first_text);

  memset(f_data.hamming_values, 0, sizeof(float) * M_BUF_SIZE);
  calculate_window(f_data.hamming_values);

  SDL_EnableScreenSaver();
  SDL_ShowWindow(w.w);

  const int ticks_per_frame = (1000.0 / vis.target_frames);
  uint64_t frame_start;
  int frame_time;

  while (!vis.quit) {
    frame_start = SDL_GetTicks64();
    SDL_SetRenderDrawBlendMode(r.r, SDL_BLENDMODE_NONE);

    render_bg(&colors.background, r.r);
    render_clear(r.r);

    if (adc.buffer && ad.position < ad.length) {
      if (get_status(&vis.dev) == SDL_AUDIO_PLAYING) {
        do_fft(&f_buffers, &f_data, &vis);
      }
    } else if (adc.buffer && ad.position >= ad.length) {
      if (get_status(&vis.dev) == SDL_AUDIO_PAUSED) {
        autoplay(&node_idx.p_node, &node_idx.node_curs, &table, &vis, &adc);
      }
    }

    switch (mode) {
    default:
      break;

    case TEXT: {
      Node *n = search_table(&table, node_idx.cur_node);
      if (n && valid_ptr(n->pbuf, n->tbuf)) {
        render_draw_text(r.r, n->tbuf, w.height, w.width);
      }
    } break;

    case PLAYBACK: {
      RenderArgs args = make_args(&f_data, &f_buffers);
      render_seek_bar(&ad.position, &ad.length, w.width, &colors.primary, r.r);
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
            int i = node_index("--", node_idx.cur_node, MAX_NODES);
            node_idx.cur_node = go_back(i, &table);
          } break;

          case SDLK_RIGHT: {
            Node *current = search_table(&table, node_idx.cur_node);
            if (current && valid_ptr(current->pbuf, current->tbuf)) {
              const size_t access = current->tbuf->info.cursor;
              const char *item_name = current->tbuf[access].text->name;
              const char *item_path = find_pathstr(item_name, current->pbuf);
              const int item_type = find_type(item_name, current->pbuf);

              switch (item_type) {
              default:
                break;

              case TYPE_FILE: {
                if (handle_type_file(item_path, &adc, &vis)) {
                  node_idx.p_node = node_idx.cur_node;
                  node_idx.node_curs = access;
                  mode = PLAYBACK;
                }

              } break;

              case TYPE_DIRECTORY: {
                int i = node_index("++", node_idx.cur_node, MAX_NODES);
                TypeDirArgs a = {.path = item_path,
                                 .tbl = &table,
                                 .r = r.r,
                                 .f = &font,
                                 .w = w.width,
                                 .text = &colors.text,
                                 .sec = &colors.secondary};
                if (handle_type_dir(i, &a)) {
                  node_idx.cur_node = i;
                }
              } break;
              }
            }
          } break;

          case SDLK_DOWN: {
            if (keymod & KMOD_SHIFT) {
              mode = PLAYBACK;
            } else {
              TextBuffer *t = search_table(&table, node_idx.cur_node)->tbuf;
              if (t) {
                move_down(t, w.height);
              }
            }
          } break;

          case SDLK_UP: {
            if (keymod & KMOD_SHIFT) {
              mode = TEXT;
            } else {
              TextBuffer *t = search_table(&table, node_idx.cur_node)->tbuf;
              if (t) {
                move_up(t);
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

  destroy_buffers(&table);

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

static void destroy_buffers(Table *t) {
  for (int i = 0; i < MAX_NODES; i++) {
    Node *n = search_table(t, i);
    if (n && n->tbuf) {
      free_text_buffer(n->tbuf, &n->tbuf->info.size);
    }

    if (n && n->pbuf) {
      free_paths(n->pbuf, &n->pbuf->size);
    }

    table_set_text(t, i, NULL);
    table_set_paths(t, i, NULL);
  }
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

static void swap_font_ptrs(Table *table, const size_t key,
                           TextBuffer *old_buffer, TextBuffer *replace) {
  table_set_text(table, key, replace);
  if (old_buffer) {
    for (size_t i = 0; i < old_buffer->info.size; i++) {
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

static RenderArgs make_args(const FFTData *d, const FFTBuffers *b) {
  RenderArgs a;
  a.smear = b->smear, a.length = &d->output_len, a.smooth = b->smoothed;
  return a;
}

static void window_resized(Window *w, const int font_size, int *char_limit) {
  SDL_GetWindowSize(w->w, &w->width, &w->height);
  *char_limit = get_char_limit(w->width, font_size);
}

static int valid_ptr(Paths *p, TextBuffer *t) {
  if ((p && t) && (p->is_valid && t->info.is_valid)) {
    return 1;
  }

  return 0;
}

static void do_fft(FFTBuffers *b, FFTData *d, const Visualizer *v) {
  clean_buffers(b);
  iter_fft(b->fft_in[access_clamp(d->buffer_access)], d->hamming_values,
           b->out_raw, M_BUF_SIZE);
  squash_to_log(b, d);
  linear_mapping(b, d, v->smearing, v->smoothing, v->target_frames);
}

static void autoplay(const size_t *playing_node, size_t *playing_cursor,
                     Table *tbl, Visualizer *v, AudioDataContainer *adc) {

  pause_device(v->dev);
  TextBuffer *t = search_table(tbl, *playing_node)->tbuf;
  Paths *p = search_table(tbl, *playing_node)->pbuf;
  if (!valid_ptr(p, t)) {
    printf("Invalid ptrs -> (Paths, TextBuffer)\n");
    return;
  }

  *playing_cursor = auto_nav_down(*playing_cursor, t->info.size);

  const char *item_path = find_pathstr(t[*playing_cursor].text->name, p);
  const int item_type = find_type(t[*playing_cursor].text->name, p);

  if (item_type != TYPE_FILE) {
    return;
  }

  if (!read_audio_file(item_path, adc)) {
    printf("Failed to read audio..\n");
    return;
  }

  set_spec(adc, v->spec);

  if (!v->dev && spec_compare(v->spec, adc)) {
    v->dev = open_device(v->spec);
  } else if (v->dev && !spec_compare(v->spec, adc)) {
    close_device(v->dev);
    v->dev = open_device(v->spec);
  }

  if (!v->dev) {
    printf("No valid audio device..\n");
    return;
  }

  resume_device(v->dev);
}

static void move_up(TextBuffer *t) {
  size_t *curs = &t->info.cursor;
  size_t *size = &t->info.size;
  if (t) {
    *curs = nav_up(*curs, *size);
  }
}

static void move_down(TextBuffer *t, const int h) {
  size_t *curs = &t->info.cursor;
  size_t *size = &t->info.size;
  if (t) {
    *curs = nav_down(*curs, *size);
  }
}

static int handle_type_file(const char *path, AudioDataContainer *adc,
                            Visualizer *v) {
  pause_device(v->dev);
  if (!read_audio_file(path, adc)) {
    printf("Failed to read audio..\n");
    return 0;
  }

  set_spec(adc, v->spec);

  if (!v->dev && spec_compare(v->spec, adc)) {
    v->dev = open_device(v->spec);
    printf("Opening new device..\n");
  } else if (v->dev && !spec_compare(v->spec, adc)) {
    printf("Replacing old device..\n");
    close_device(v->dev);
    v->dev = open_device(v->spec);
  }

  if (!v->dev) {
    printf("No valid device..\n");
    return 0;
  }

  printf("Resuming device.\n");
  resume_device(v->dev);
  return 1;
}
static int handle_type_dir(const int i, TypeDirArgs *args) {
  // If this index is the same as the playing node it will just overwrite and
  // autoplay from the new playing node, which will clamp the index to the
  // relevant node. I could fix this with some pointer stuff but i'm too lazy
  // at the moment.
  SDL_Renderer *r = args->r;
  Font *f = args->f;
  const int w = args->w;
  const SDL_Color *text = args->text;
  const SDL_Color *sec = args->sec;

  Node *old = search_table(args->tbl, i);
  if (old) {
    if (old->tbuf) {
      old->tbuf = free_text_buffer(old->tbuf, &old->tbuf->info.size);
    }

    if (old->pbuf) {
      old->pbuf = free_paths(old->pbuf, &old->pbuf->size);
    }
  }

  Paths *p = fs_search(args->path);
  TextBuffer *t = create_fonts(p, r, f, w, text, sec);

  table_set_paths(args->tbl, i, p);
  table_set_text(args->tbl, i, t);

  if (p && t) {
    return 1;
  }

  return 0;
}

static int go_back(const int i, Table *tbl) {
  Paths *p = search_table(tbl, i)->pbuf;
  TextBuffer *t = search_table(tbl, i)->tbuf;

  if (valid_ptr(p, t)) {
    return i;
  }

  // default to 0 as an index if failed the check.
  return 0;
}
