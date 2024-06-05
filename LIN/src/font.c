#include "../inc/font.h"
#include "../inc/music_visualizer.h"

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
create_active_song_font(FontContext* Fnt, FileState* FS, SDL_Renderer* r) {

  Fnt->active->tex  = destroy_texture(Fnt->active->tex);
  Fnt->active->text = free_ptr(Fnt->active->text);

  Fnt->active->ready = FALSE;
  int file_index     = FS->file_index;
  int size           = strlen(FS->files[file_index]);
  Fnt->active->text  = malloc(sizeof(char*) * size);
  if (Fnt->active->text == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate active song char buffer\n", strerror(errno));
    return -1;
  }
  strcpy(Fnt->active->text, FS->files[file_index]);
  clean_text(Fnt->active->text);
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
  Fnt->sf_arr = malloc(FS->file_count * sizeof(FontData));
  if (Fnt->sf_arr == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate font surface array", strerror(errno));
    Fnt->state->song_fonts_created = 0;
    return -1;
  }

  int y_pos                  = 0;
  Fnt->pos->song_list_height = 0;

  for (int s = 0; s < FS->file_count; s++) {

    char text[PATH_MAX];
    strcpy(text, FS->files[s]);
    clean_text(text);

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

    SDL_Surface** file_surf = &Fnt->sf_arr[s].font_surface;
    SDL_Rect      sdl_rect  = { 50, y_pos, (*file_surf)->w, (*file_surf)->h };

    Fnt->sf_arr[s].id        = s;
    Fnt->sf_arr[s].text      = FS->files[s];
    Fnt->sf_arr[s].font_rect = sdl_rect;

    y_pos += CONST_YPOS;
    *file_surf = destroy_surface(*file_surf);
  }

  Fnt->state->song_fonts_created = TRUE;
  Fnt->pos->song_list_height     = y_pos;
  return 0;
}

int
create_dir_fonts(FontContext* Fnt, DirState* DS, SDL_Renderer* r) {

  Fnt->df_arr = malloc(DS->dir_count * sizeof(FontData));
  if (Fnt->df_arr == NULL) {
    PRINT_STR_ERR(stderr, "Could not allocate font surface array", strerror(errno));
    Fnt->state->dir_fonts_created = FALSE;
    return -1;
  }

  int y_pos                 = 0;
  Fnt->pos->dir_list_height = 0;

  for (int s = 0; s < DS->dir_count; s++) {

    char text[PATH_MAX];
    strcpy(text, DS->directories[s]);
    clean_text(text);

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

    SDL_Surface** dir_surf = &Fnt->df_arr[s].font_surface;
    SDL_Rect      sdl_rect = { 50, y_pos, (*dir_surf)->w, (*dir_surf)->h };

    Fnt->df_arr[s].id        = s;
    Fnt->df_arr[s].text      = DS->directories[s];
    Fnt->df_arr[s].font_rect = sdl_rect;

    y_pos += CONST_YPOS;
    *dir_surf = destroy_surface(*dir_surf);
  }

  Fnt->state->dir_fonts_created = TRUE;
  Fnt->pos->dir_list_height     = y_pos;
  return 0;
}

void
clear_font_bgs(FontData* arr[], int len) {
  for (int i = 0; i < len; i++) {
    (*arr)[i].has_bg = 0;
  }
}

void
create_dir_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC) {

  FontContext*  FntPtr     = SDLC->FntPtr;
  SDLContainer* SDLCntrPtr = SDLC->container;
  DirState*     DSPtr      = SDLC->FCPtr->dir_state;

  int one_tenth = (int)(SDLCntrPtr->win_height * 0.05);

  if (point_in_rect(mouse_x, mouse_y, SDLCntrPtr->dir_viewport)) {
    const int  mouse_arr[] = { mouse_x, mouse_y - one_tenth };
    int        dir_count   = DSPtr->dir_count;
    FontData** df_arr      = &FntPtr->df_arr;
    FontData*  df          = get_struct(df_arr, mouse_arr, dir_count);
    if (df == NULL) {
      return;
    }
    df->has_bg = TRUE;
    /*Y pos is set to 0 as it gets determined by the y_pos in the rendering function*/
    SDL_Rect bg = { df->font_rect.x - 5, 0, df->font_rect.w + 10, df->font_rect.h + 10 };
    df->font_bg = bg;
  } else {
    clear_font_bgs(&FntPtr->df_arr, DSPtr->dir_count);
  }
} /*create_dir_text_bg*/

void
create_song_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC) {

  FontContext*  FntPtr     = SDLC->FntPtr;
  SDLContainer* SDLCntrPtr = SDLC->container;
  FileState*    FSPtr      = SDLC->FCPtr->file_state;

  if (point_in_rect(mouse_x, mouse_y, SDLCntrPtr->song_viewport)) {
    int offset_y = SDLC->mouse->mouse_offset_y;

    const int  mouse_arr[] = { (mouse_x), (mouse_y - offset_y) };
    int        file_count  = FSPtr->file_count;
    FontData** sf_arr      = &FntPtr->sf_arr;
    FontData*  sf          = get_struct(sf_arr, mouse_arr, file_count);
    if (sf == NULL) {
      return;
    }
    sf->has_bg = TRUE;
    /*Y pos is set to 0 as it gets determined by the y_pos in the rendering function*/
    SDL_Rect bg = { sf->font_rect.x - 5, 0, sf->font_rect.w + 10, sf->font_rect.h + 10 };
    sf->font_bg = bg;
  } else {
    clear_font_bgs(&FntPtr->sf_arr, FSPtr->file_count);
  }
} /*create_song_text_bg*/

void
clean_text(char text[]) {
  int j, k;
  for (j = 0, k = 0; text[j] != '\0'; j++) {
    if (text[j] == '_') {
      text[j] = ' ';
    }

    if (text[j] != '_') {
      text[k++] = text[j];
    }
  }
  text[k] = '\0';
}

void
clear_fonts(FontContext* FntPtr, FileContext* FCPtr) {

  i8 files_exist        = FCPtr->file_state->files_exist;
  i8 song_fonts_created = FntPtr->state->song_fonts_created;

  if (song_fonts_created && files_exist) {
    int file_count = FCPtr->file_state->file_count;
    for (int i = 0; i < file_count; i++) {
      destroy_texture(FntPtr->sf_arr[i].font_texture);
    }
  }

  i8 dirs_exist        = FCPtr->dir_state->dirs_exist;
  i8 dir_fonts_created = FntPtr->state->dir_fonts_created;

  if (dir_fonts_created && dirs_exist) {
    int dir_count = FCPtr->dir_state->dir_count;
    for (int i = 0; i < dir_count; i++) {
      destroy_texture(FntPtr->df_arr[i].font_texture);
    }
  }

  FntPtr->active->tex  = destroy_texture(FntPtr->active->tex);
  FntPtr->active->text = free_ptr(FntPtr->active->text);
}

void
clear_existing_list(FontData** sf_arr, int song_fonts_created, FileState* FSPtr, char* selection) {
  FSPtr->selected_dir = selection;
  if (FSPtr->files_exist && FSPtr->file_count > 0) {
    for (int i = 0; i < FSPtr->file_count; i++) {
      FSPtr->files[i] = free_ptr(FSPtr->files[i]);
    }

    FSPtr->file_count = 0;
  }

  if (FSPtr->files_exist) {
    FSPtr->files = free_ptr(FSPtr->files);
  }

  if (song_fonts_created) {
    for (int i = 0; i < FSPtr->file_count; i++) {
      (*sf_arr)[i].font_texture = destroy_texture((*sf_arr)[i].font_texture);
    }
  }
} /*clear_existing_list*/
