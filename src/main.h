#ifndef MAIN_H
#define MAIN_H
#include "fontdef.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {DIRECTORIES = 0, SONGS = 1, PLAYBACK = 2} USER_STATE;

#define FONT_PATH "dogicapixel.ttf"
#define DEFAULT_TABLE_SIZE 1024

#define WIN_W 600
#define WIN_H 400

typedef struct {
    bool quit;
    int current_state;
    SDL_Color primary;
    SDL_Color secondary;
    SDL_Color background;
    SDL_Color text;
    SDL_Color text_bg;
} Visualizer;

typedef struct {
    SDL_Window *w;
    int width,height;
}Window;

typedef struct{
    SDL_Renderer *r;
    size_t dir_locn;
    size_t song_locn;
    size_t title_limit;
    size_t draw_amount;
} Renderer;

typedef struct {
    size_t dir_list_index;
    size_t song_list_index;
    size_t song_cursor;
    size_t dir_cursor;
}Events;

typedef struct {
    TTF_Font *font;
    int char_limit;
    int size;
}Font;

extern Font font;
extern Renderer rend;
extern Window win;
extern Visualizer vis;
extern Events key;

void *scp(void* ptr);
int scc(int code);

int get_title_limit(int height);
int get_char_limit(int width);

TextBuffer** create_directory_fonts(char **dir_buf, const size_t dir_count, size_t *txt_buf_size,const size_t *sub_buf_size);

void render_bg(void);
void render_draw_text(Text **text_buf, const size_t *cursor,  const size_t *size);
void render_clear(void);
void render_present(void);


size_t nav_down(size_t *cursor, size_t *locn, size_t max_length);
size_t nav_up(size_t *cursor, size_t *locn, size_t max_length);

#endif // MAIN_H
