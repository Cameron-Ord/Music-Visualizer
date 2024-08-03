#include "../inc/font.h"
#include "../inc/audio.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"
#include "../inc/utils.h"

SDL_Surface*
create_font_surface(TTF_Font** font, SDL_Color color, char* text) {
  SDL_Surface* text_surface = TTF_RenderText_Blended((*font), text, color);
  if (text_surface == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return NULL;
  }
  return text_surface;
}

SDL_Texture*
create_font_texture(SDL_Renderer* r, SDL_Surface* surface) {
  SDL_Texture* font_texture = SDL_CreateTextureFromSurface(r, surface);
  if (font_texture == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    SDL_FreeSurface(surface);
    return NULL;
  }
  return font_texture;
}

int
create_colours_fonts(FontContext* Fnt, Theme* Themes[], SDL_Renderer* r) {
  for (int s = 0; s < COLOUR_LIST_SIZE; s++) {
    Fnt->colours_list[s].has_bg = FALSE;
    Fnt->colours_list[s].font_surface
        = create_font_surface(&Fnt->context_data->font, Fnt->context_data->color, Themes[s]->name);
    if (Fnt->colours_list[s].font_surface == NULL) {
      return -1;
    }

    Fnt->colours_list[s].font_texture = create_font_texture(r, Fnt->colours_list[s].font_surface);
    if (Fnt->colours_list[s].font_texture == NULL) {
      return -1;
    }

    SDL_Surface* surf = Fnt->colours_list[s].font_surface;
    SDL_Rect     rect = { 0, 0, surf->w, surf->h };

    Fnt->colours_list[s].id        = s;
    Fnt->colours_list[s].text      = Themes[s]->name;
    Fnt->colours_list[s].font_rect = rect;

    surf = destroy_surface(surf);
  }

  Fnt->state->col_fonts_created = TRUE;
  return 0;
}

int
create_active_song_font(FontContext* Fnt, FileState* FS, SDL_Renderer* r) {
  Fnt->active->tex = destroy_texture(Fnt->active->tex);
  free_ptr(Fnt->active->text);

  Fnt->active->ready = FALSE;

  int    file_index = FS->file_index;
  size_t size       = strlen(FS->files[file_index]);

  Fnt->active->text = malloc(sizeof(char*) * size);
  if (Fnt->active->text == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate active song char buffer\n", strerror(errno));
    return -1;
  }

  strcpy(Fnt->active->text, FS->files[file_index]);
  clean_text(Fnt->active->text, "False");
  char* text = Fnt->active->text;

  Fnt->active->surf = create_font_surface(&Fnt->context_data->font, Fnt->context_data->color, text);
  if (Fnt->active->surf == NULL) {
    return -1;
  }

  Fnt->active->tex = create_font_texture(r, Fnt->active->surf);
  if (Fnt->active->tex == NULL) {
    return -1;
  }

  SDL_Surface** font_surf_ptr = &Fnt->active->surf;
  SDL_Rect      font_rect     = { 0, 0, (*font_surf_ptr)->w, (*font_surf_ptr)->h };

  Fnt->active->rect        = font_rect;
  Fnt->active->offset_rect = font_rect;
  Fnt->active->id          = 1;

  *font_surf_ptr = destroy_surface(*font_surf_ptr);

  Fnt->active->ready = TRUE;
  return 0;
}

int
create_song_fonts(FontContext* Fnt, FileState* FS, SDL_Renderer* r) {
  Fnt->state->song_fonts_created = FALSE;

  Fnt->sf_arr = malloc(FS->file_count * sizeof(FontData));
  if (Fnt->sf_arr == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate font surface array", strerror(errno));
    return -1;
  }

  for (int s = 0; s < FS->file_count; s++) {

    char text[PATH_MAX];
    strcpy(text, FS->files[s]);
    clean_text(text, "True");

    Fnt->sf_arr[s].has_bg = FALSE;
    Fnt->sf_arr[s].font_surface
        = create_font_surface(&Fnt->context_data->font, Fnt->context_data->color, text);
    if (Fnt->sf_arr[s].font_surface == NULL) {
      Fnt->state->song_fonts_created = FALSE;
      return -1;
    }

    Fnt->sf_arr[s].font_texture = create_font_texture(r, Fnt->sf_arr[s].font_surface);
    if (Fnt->sf_arr[s].font_texture == NULL) {
      Fnt->state->song_fonts_created = FALSE;
      return -1;
    }

    SDL_Surface* surf = Fnt->sf_arr[s].font_surface;
    SDL_Rect     rect = { 0, 0, surf->w, surf->h };

    Fnt->sf_arr[s].id        = s;
    Fnt->sf_arr[s].text      = FS->files[s];
    Fnt->sf_arr[s].font_rect = rect;

    surf = destroy_surface(surf);
  }
  Fnt->state->song_fonts_created = TRUE;
  return 0;
}

int
create_dir_fonts(FontContext* Fnt, DirState* DS, SDL_Renderer* r) {
  Fnt->state->dir_fonts_created = FALSE;

  Fnt->df_arr = malloc(DS->dir_count * sizeof(FontData));
  if (Fnt->df_arr == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate font surface array", strerror(errno));
    return -1;
  }

  for (int s = 0; s < DS->dir_count; s++) {

    char text[PATH_MAX];
    strcpy(text, DS->directories[s]);
    clean_text(text, "True");

    Fnt->df_arr[s].has_bg = FALSE;
    Fnt->df_arr[s].font_surface
        = create_font_surface(&Fnt->context_data->font, Fnt->context_data->color, text);
    if (Fnt->df_arr[s].font_surface == NULL) {
      Fnt->state->dir_fonts_created = FALSE;
      return -1;
    }

    Fnt->df_arr[s].font_texture = create_font_texture(r, Fnt->df_arr[s].font_surface);
    if (Fnt->df_arr[s].font_texture == NULL) {
      Fnt->state->dir_fonts_created = FALSE;
      return -1;
    }

    SDL_Surface* surf = Fnt->df_arr[s].font_surface;
    SDL_Rect     rect = { 0, 0, surf->w, surf->h };

    Fnt->df_arr[s].id        = s;
    Fnt->df_arr[s].text      = DS->directories[s];
    Fnt->df_arr[s].font_rect = rect;

    surf = destroy_surface(surf);
  }

  Fnt->state->dir_fonts_created = TRUE;
  return 0;
}

void
clear_font_bgs(FontData arr[], int len) {
  for (int i = 0; i < len; i++) {
    arr[i].has_bg = FALSE;
  }
}

void
create_col_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT) {
  SDLContainer* Cont = SDLC->container;
  SDL_Rect      vp   = { 0, 0, Cont->win_width, Cont->win_height };

  if (point_in_rect(mouse_x, mouse_y, vp)) {
    const int mouse_arr[] = { mouse_x, mouse_y };
    FontData* col_ptr     = FNT->colours_list;
    FontData* col_rtn     = get_struct(col_ptr, mouse_arr, COLOUR_LIST_SIZE);

    if (col_rtn == NULL) {
      return;
    }

    col_rtn->has_bg = TRUE;
  } else {
    clear_font_bgs(FNT->colours_list, COLOUR_LIST_SIZE);
  }
}

void
create_dir_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT,
                   FileContext* FC) {

  SDLContainer* Cont  = SDLC->container;
  SDLViewports* Vps   = Cont->vps;
  DirState*     DSPtr = FC->dir_state;

  if (point_in_rect(mouse_x, mouse_y, Vps->dir_vp)) {

    const int mouse_arr[] = { mouse_x, mouse_y };
    int       dir_count   = DSPtr->dir_count;
    FontData* df_arr      = FNT->df_arr;
    FontData* df          = get_struct(df_arr, mouse_arr, dir_count);

    if (df == NULL) {
      return;
    }

    df->has_bg = TRUE;
    /*Y pos is set to 0 as it gets determined by the y_pos in the rendering function*/
  } else {
    clear_font_bgs(FNT->df_arr, DSPtr->dir_count);
  }
} /*create_dir_text_bg*/

void
create_song_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC, FontContext* FNT,
                    FileContext* FC) {
  SDLContainer* Cont  = SDLC->container;
  SDLViewports* Vps   = Cont->vps;
  FileState*    FSPtr = FC->file_state;

  if (point_in_rect(mouse_x, mouse_y, Vps->song_vp)) {
    int offset_x = SDLC->mouse->mouse_offset_x;
    int offset_y = SDLC->mouse->mouse_offset_y;

    const int mouse_arr[] = { (mouse_x - offset_x), (mouse_y - offset_y) };
    int       file_count  = FSPtr->file_count;
    FontData* sf_arr      = FNT->sf_arr;

    FontData* sf = get_struct(sf_arr, mouse_arr, file_count);
    if (sf == NULL) {
      return;
    }

    sf->has_bg = TRUE;
    /*Y pos is set to 0 as it gets determined by the y_pos in the rendering function*/
  } else {
    clear_font_bgs(FNT->sf_arr, FSPtr->file_count);
  }
} /*create_song_text_bg*/

void
clean_text(char text[], char elipsis[]) {
  int j, k;
  for (j = 0, k = 0; text[j] != '\0'; j++) {
    if (text[j] == '_') {
      text[j] = ' ';
    }

    if (text[j] != '_') {
      text[k++] = text[j];
    }
  }

  if (strcmp(elipsis, "True") == 0) {
    size_t limit = 52;
    if (strlen(text) > limit) {
      text[limit]     = '.';
      text[limit + 1] = '.';
      text[limit + 2] = '.';
    }

    text[limit + 3] = '\0';
    return;
  }

  text[k] = '\0';
}

FontData*
get_struct(FontData arr[], const int mouse_arr[], int len) {
  for (int i = 0; i < len; i++) {
    SDL_Rect rect = arr[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], rect)) {
      return &arr[i];
    } else {
      arr[i].has_bg = 0;
    }
  }
  return NULL;
} /*get_struct*/

void
resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  int win_width    = SDLC->container->win_width;

  TTF_Font** font = &FNT->context_data->font;

  const f32 one_thousandth = 0.016;

  const int MIN_FONT_SIZE = 12;
  const int MAX_FONT_SIZE = 20;

  int new_font_size = win_width * one_thousandth;

  if (new_font_size > MAX_FONT_SIZE) {
    new_font_size = MAX_FONT_SIZE;
  }

  if (new_font_size < MIN_FONT_SIZE) {
    new_font_size = MIN_FONT_SIZE;
  }

  FNT->context_data->font_size = new_font_size;

  TTF_SetFontSize(*font, new_font_size);

  int file_count = FC->file_state->file_count;
  destroy_song_fonts(FNT, file_count);
  create_song_fonts(FNT, FC->file_state, SDLC->r);

  int dir_count = FC->dir_state->dir_count;
  destroy_dir_fonts(FNT, dir_count);
  create_dir_fonts(FNT, FC->dir_state, SDLC->r);

  destroy_colours_fonts(FNT);
  create_colours_fonts(FNT, SDLC->container->theme->themes, SDLC->r);

  if (playing_song) {
    // this kinda seems confusing, that it lacks the destroy function. It just destroys the existing
    // textures/text inside the function itself before creating new ones. I just didn't seperate that step for
    // this one.
    create_active_song_font(FNT, FC->file_state, SDLC->r);
  }
}
