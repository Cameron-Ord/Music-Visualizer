#ifndef FONT_H
#define FONT_H

#include "macro.h"
#include "types.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

typedef enum {
  ExTiny = 6,
  ExxSM  = 8,
  ExSM   = 10,
  SM     = 12,
  MED    = 14,
  LRG    = 16,
  XLRG   = 18,
  XXLRG  = 20,
} FONTSIZE_BINDS;

struct Positions {
  int dir_list_pos;
  int dir_list_offset;
  int dir_list_height;

  int song_list_pos;
  int song_list_offset;
  int song_list_height;
};

struct FontData {
  SDL_Texture* font_texture;
  SDL_Surface* font_surface;
  SDL_Rect     font_rect;
  SDL_Rect     font_bg;
  char*        text;
  i32          id;
  i8           has_bg;
};

struct ActiveSong {
  int          x;
  int          y;
  SDL_Rect     rect;
  SDL_Rect     offset_rect;
  SDL_Rect     vp;
  SDL_Texture* tex;
  SDL_Surface* surf;
  char*        text;
  i32          id;
  i8           ready;
};

struct FontState {
  i8  dir_fonts_created;
  i8  song_fonts_created;
  int initialized;
};

struct TTFData {
  TTF_Font* font;
  char*     font_path;
  SDL_Color color;
};

struct FontContext {
  struct TTFData*    context_data;
  struct FontState*  state;
  struct FontData*   df_arr;
  struct FontData*   sf_arr;
  struct Positions*  pos;
  struct ActiveSong* active;
};

void         clear_fonts(FontContext* FntPtr, FileContext* FCPtr);
SDL_Texture* create_font_texture(SDL_Renderer* r, SDL_Surface* surface);
SDL_Surface* create_font_surface(TTF_Font** font, SDL_Color color, char* text);
void         set_active_song_title(FontContext* FntPtr, int win_width, int win_height);
void         draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve);
void         update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max);
void         update_font_surface(SDL_Rect* font_rect, int c_width, int c_height);
void         clear_existing_list(FontData** sf_arr, int song_fonts_created, FileState* FS, char* selection);
void         resize_fonts(SDLContext* SDLC);
int          create_dir_fonts(FontContext* Fnt, DirState* DS, SDL_Renderer* r);
int          create_song_fonts(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
void         create_dir_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC);
void         create_song_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC);
void         clear_font_bgs(FontData* arr[], int len);
void         baseline_context_data(TTFData* fontcntxt);
void         baseline_font_state(FontState* state);
void         baseline_pos(Positions* pos);
int          initialize_TTF();
int          open_font(TTFData* context_data);
int          create_active_song_font(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
void         clean_text(char text[]);
#endif
