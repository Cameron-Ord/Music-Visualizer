#ifndef FONT_H
#define FONT_H

#include "macdef.h"
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
  SDL_Texture* tex;
  SDL_Surface* surf;
  char*        text;
  i32          id;
  i8           ready;
};

struct FontState {
  i8  dir_fonts_created;
  i8  song_fonts_created;
  i8  col_fonts_created;
  int initialized;
};

struct TTFData {
  TTF_Font* font;
  SDL_Color color;
  int       font_size;
};

struct FontContext {
  struct TTFData*    context_data;
  struct FontState*  state;
  struct FontData*   df_arr;
  struct FontData*   sf_arr;
  struct FontData    colours_list[COLOUR_LIST_SIZE];
  struct Positions*  pos;
  struct ActiveSong* active;
};

void         destroy_dir_fonts(FontContext* Fnt, int dir_count);
void         destroy_song_fonts(FontContext* Fnt, int file_count);
void         reset_songlist_pos(Positions* pos);
FontData*    get_struct(FontData arr[], const int mouse_arr[], int len);
SDL_Texture* create_font_texture(SDL_Renderer* r, SDL_Surface* surface);
SDL_Surface* create_font_surface(TTF_Font** font, SDL_Color color, char* text);
void         update_font_surface(SDL_Rect* font_rect, int c_width, int c_height);
void         clear_existing_list(FontData* sf_arr, int song_fonts_created, FileState* FS, char* selection);
void         resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT);
int          create_dir_fonts(FontContext* Fnt, DirState* DS, SDL_Renderer* r);
int          create_song_fonts(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
int          create_colours_fonts(FontContext* Fnt, Theme* Themes[], SDL_Renderer* r);
int          destroy_colours_fonts(FontContext* Fnt);
void         create_dir_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT,
                                FileContext* FC);
void         create_song_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT,
                                 FileContext* FC);
void         clear_font_bgs(FontData arr[], int len);
int          initialize_TTF();
int          open_font(TTFData* context_data);
int          create_active_song_font(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
void         clean_text(char text[], char elipsis[]);
void         create_col_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT);
#endif
