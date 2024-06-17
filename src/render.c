#include "../inc/render.h"
#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"
#include <complex.h>

void*
destroy_surface(SDL_Surface* surf) {
  if (surf != NULL) {
    SDL_FreeSurface(surf);
  }
  return NULL;
}

void*
destroy_texture(SDL_Texture* tex) {
  if (tex != NULL) {
    SDL_DestroyTexture(tex);
  }
  return NULL;
}

void
clear_render(SDL_Renderer* r) {
  SDL_RenderClear(r);
}

void
render_background(SDL_Renderer* r) {
  SDL_SetRenderDrawColor(r, 40, 42, 54, 255);
}

void
present_render(SDL_Renderer* r) {
  SDL_RenderPresent(r);
}

void
update_window_size(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w) {
  int* win_width  = &Cont->win_width;
  int* win_height = &Cont->win_height;
  SDL_GetWindowSize(w, win_width, win_height);
}

void
set_stopped_viewports(SDLContext* SDLC, SDL_Rect* dir_vp_ptr, SDL_Rect* song_vp_ptr,
                      SDL_Rect* settings_vp_ptr) {
  SDLContainer* Cont  = SDLC->container;
  SDLMouse*     Mouse = SDLC->mouse;

  int width  = Cont->win_width;
  int height = Cont->win_height;

  SDL_Rect tmp_dir_vp  = { 0, 0, width * 0.5, height };
  SDL_Rect tmp_song_vp = { width * 0.5, 0, width * 0.5, height };
  SDL_Rect tmp_sett    = { 0, 0, width, height };

  *dir_vp_ptr      = tmp_dir_vp;
  *song_vp_ptr     = tmp_song_vp;
  *settings_vp_ptr = tmp_sett;

  Mouse->mouse_offset_y = 0;
  Mouse->mouse_offset_x = width * 0.5;

  SDLViewports* Vps = SDLC->container->vps;

  Vps->dir_vp      = tmp_dir_vp;
  Vps->song_vp     = tmp_song_vp;
  Vps->settings_vp = tmp_sett;
}

void
set_playing_viewports(SDLContext* SDLC, SDL_Rect* control_vp_ptr, SDL_Rect* viz_vp_ptr,
                      SDL_Rect* settings_vp_ptr) {
  SDLContainer* Cont = SDLC->container;

  int height = Cont->win_height;
  int width  = Cont->win_width;

  SDL_Rect tmp_cntrl = { 0, 0, width, height * 0.25 };
  SDL_Rect tmp_viz   = { 0, height * 0.25, width, height * 0.75 };
  SDL_Rect tmp_sett  = { 0, 0, width, height };

  *control_vp_ptr  = tmp_cntrl;
  *viz_vp_ptr      = tmp_viz;
  *settings_vp_ptr = tmp_sett;

  SDLViewports* Vps = SDLC->container->vps;

  Vps->visualization_vp = tmp_viz;
  Vps->settings_vp      = tmp_sett;
  Vps->controls_vp      = tmp_cntrl;
}

void
render_set_rgba_sliders(SDLContext* SDLC, SDL_Rect* vp) {
  SDLColours* Col = SDLC->container->theme;

  const size_t len      = 3;
  const size_t fields   = 4;
  const size_t elements = len * fields;
  const u8     max      = 255;

  SDL_Color* colour_struct_array[] = { &Col->primary, &Col->secondary, &Col->tertiary };

  const f32 increment = 1.0 / elements;
  f32       factor    = 1.0 / elements;

  for (size_t i = 0; i < len; i++) {
    SDL_Color* rgba_field   = colour_struct_array[i];
    u8         rgba_array[] = { rgba_field->r, rgba_field->g, rgba_field->b, rgba_field->a };
    for (size_t j = 0; j < fields; j++) {
      Col->normalized_positions[i][j] = (float)rgba_array[j] / max;
      Col->scaled_positions[i][j]     = vp->w * ((float)rgba_array[j] / max);

      int      x   = Col->scaled_positions[i][j] - SCROLLBAR_OFFSET;
      SDL_Rect bar = { x, vp->h * factor, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };

      Col->scroll_bars[i][j] = bar;
      factor += increment;
    }
  }
}

void
render_draw_rgba_sliders(SDLContext* SDLC, SDL_Rect* vp) {

  SDLColours* Col = SDLC->container->theme;

  const size_t len          = 3;
  const size_t fields_count = 4;

  SDL_RenderSetViewport(SDLC->r, vp);
  SDL_SetRenderDrawColor(SDLC->r, Col->primary.r, Col->primary.g, Col->primary.b, Col->primary.a);

  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < fields_count; j++) {
      SDL_RenderFillRect(SDLC->r, &Col->scroll_bars[i][j]);
    }
  }
}

void
render_set_gear(SDLContainer* Cont, SettingsGear* gear) {
  int w        = 64;
  int x_offset = Cont->win_width - w;

  int padding_x = 10;
  int padding_y = 10;

  set_rect(&gear->rect, NULL, x_offset + (w / 2) - padding_x, 0 + padding_y);
}

void
render_draw_gear(SDL_Renderer* r, SettingsGear* gear) {
  SDL_RenderSetViewport(r, NULL);
  SDL_RenderCopy(r, gear->tex, NULL, &gear->rect);
}

void
render_set_gear_active(SDLContainer* Cont, SettingsGear* gear, SDL_Rect* vp) {
  int w        = 64;
  int h        = 64;
  int y        = vp->h * 0.75;
  int x_offset = vp->w * 0.4;

  set_rect(&gear->rect, NULL, x_offset - (w / 2), y - (h / 2));
}

void
render_draw_gear_active(SDL_Renderer* r, SettingsGear* gear, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  SDL_RenderCopy(r, gear->tex, NULL, &gear->rect);
}

void
render_bars(SDLContext* SDLC, SDL_Rect* vp) {

  int out_len = SDLC->FTPtr->fft_data->output_len;

  if (out_len == 0) {
    fprintf(stdout, "OUTPUT LENGTH 0 - SOMETHING HAS GONE HORRIBLY WRONG\n");
    PlaybackState* pb = SDLC->SSPtr->pb_state;

    pb->hard_stop = TRUE;
    stop_playback(NULL, pb, &SDLC->audio_dev);
    return;
  }

  SDL_RenderSetViewport(SDLC->r, vp);

  f32* out        = SDLC->FTPtr->fft_buffers->smoothed_prim;
  i16  cell_width = vp->w / out_len;
  int  h          = vp->h;

  for (int i = 0; i < out_len; ++i) {
    float t          = out[i];
    int   x_pos      = (i * (int)(cell_width + cell_width / 2));
    int   y_pos      = h - ((float)h * t);
    int   bar_height = (float)h * t;

    SDL_Rect sample_plus = { x_pos, y_pos, cell_width, bar_height };

    SDL_SetRenderDrawColor(SDLC->r, 189, 147, 249, 255);
    SDL_RenderFillRect(SDLC->r, &sample_plus);
  }
}

void
render_set_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count, SDL_Rect* vp) {
  Positions*   Pos   = FNT->pos;
  ListLimiter* LLmtr = SDLC->container->list_limiter;

  const int height_offset = 5;

  size_t last_index = LLmtr->dir_first_index + 8;
  if (last_index > (size_t)dir_count) {
    last_index = (size_t)dir_count;
  }

  LLmtr->dir_last_index = last_index;

  const f32 increment = 1.0 / (8 + 1);
  f32       factor    = 1.0 / (8 + 1);

  FontData* df_arr = FNT->df_arr;

  for (int i = 0; i < dir_count; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;
    SDL_Rect* font_bg   = &df_arr[i].font_bg;

    font_rect->y = 0;
    font_rect->x = 0;
  }

  for (size_t i = LLmtr->dir_first_index; i < last_index; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;
    SDL_Rect* font_bg   = &df_arr[i].font_bg;

    font_rect->y = factor * vp->h;
    font_rect->x = 25;

    if (df_arr[i].has_bg) {
      int      bg_y = (factor * vp->h) - height_offset;
      SDL_Rect bg   = { font_rect->x - 5, bg_y, font_rect->w + 10, font_rect->h + 10 };

      *font_bg = bg;
    }
    factor += increment;
  }
}

void
render_draw_dir_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp) {
  SDL_RenderSetViewport(SDLC->r, vp);

  ListLimiter* LLmtr  = SDLC->container->list_limiter;
  SDLColours*  Col    = SDLC->container->theme;
  FontData*    df_arr = FNT->df_arr;

  for (size_t i = LLmtr->dir_first_index; i < LLmtr->dir_last_index; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;
    SDL_Rect* font_bg   = &df_arr[i].font_bg;

    if (df_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, Col->tertiary.r, Col->tertiary.g, Col->tertiary.b, Col->tertiary.a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    SDL_RenderCopy(SDLC->r, df_arr[i].font_texture, NULL, font_rect);
  }
}

void
render_set_song_list(SDLContext* SDLC, FontContext* FNT, int file_count, SDL_Rect* vp) {
  Positions*   Pos   = FNT->pos;
  ListLimiter* LLmtr = SDLC->container->list_limiter;

  const int height_offset = 5;

  size_t last_index = LLmtr->song_first_index + 8;
  if (last_index > (size_t)file_count) {
    last_index = (size_t)file_count;
  }

  LLmtr->song_last_index = last_index;

  const f32 increment = 1.0 / (8 + 1);
  f32       factor    = 1.0 / (8 + 1);

  FontData* sf_arr = FNT->sf_arr;

  for (int i = 0; i < file_count; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;
    SDL_Rect* font_bg   = &sf_arr[i].font_bg;
    font_rect->y        = 0;
    font_rect->x        = 0;
  }

  for (size_t i = LLmtr->song_first_index; i < last_index; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;
    SDL_Rect* font_bg   = &sf_arr[i].font_bg;

    font_rect->y = factor * vp->h;
    font_rect->x = 25;

    if (sf_arr[i].has_bg) {
      int      bg_y = (factor * vp->h) - height_offset;
      SDL_Rect bg   = { font_rect->x - 5, bg_y, font_rect->w + 10, font_rect->h + 10 };

      *font_bg = bg;
    }
    factor += increment;
  }
}

void
render_draw_song_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp) {
  SDL_RenderSetViewport(SDLC->r, vp);

  ListLimiter* LLmtr  = SDLC->container->list_limiter;
  SDLColours*  Col    = SDLC->container->theme;
  FontData*    sf_arr = FNT->sf_arr;

  for (size_t i = LLmtr->song_first_index; i < LLmtr->song_last_index; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;
    SDL_Rect* font_bg   = &sf_arr[i].font_bg;

    if (sf_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, Col->tertiary.r, Col->tertiary.g, Col->tertiary.b, Col->tertiary.a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    SDL_RenderCopy(SDLC->r, sf_arr[i].font_texture, NULL, font_rect);
  }
}

void
set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud, SDL_Rect* vp) {
  int ttl_length       = Aud->wav_len;
  int current_position = Aud->audio_pos;

  int line_x     = vp->w * 0.20;
  int sub_amount = (vp->w * 0.20) / 2;

  SkBar->normalized_pos = ((float)current_position / (float)ttl_length);
  SkBar->current_pos    = SkBar->normalized_pos * (vp->w * 0.20);

  int x = SkBar->current_pos - SCROLLBAR_OFFSET;
  int y = vp->h * 0.75;

  SDL_Rect sk_box = { x + sub_amount, y - (SCROLLBAR_HEIGHT / 2), SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };
  SDL_Rect sk_line
      = { line_x - sub_amount, (y - (SCROLLBAR_HEIGHT / 2)) + SCROLLBAR_HEIGHT_OFFSET, vp->w * 0.20, 2 };

  SkBar->seek_box  = sk_box;
  SkBar->seek_line = sk_line;
}

void
set_vol_bar(SDLContainer* Cont, VolBar* VBar, AudioData* Aud, SDL_Rect* vp) {
  VBar->current_pos = Aud->volume * (vp->w * 0.20);

  int line_x     = vp->w * 0.80;
  int sub_amount = (vp->w * 0.20) / 2;

  int x = VBar->current_pos - SCROLLBAR_OFFSET;
  int y = vp->h * 0.75;

  SDL_Rect sk_box
      = { x + (line_x - sub_amount), y - (SCROLLBAR_HEIGHT / 2), SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };
  SDL_Rect sk_line
      = { line_x - sub_amount, (y - (SCROLLBAR_HEIGHT / 2)) + SCROLLBAR_HEIGHT_OFFSET, vp->w * 0.20, 2 };

  VBar->seek_box  = sk_box;
  VBar->seek_line = sk_line;
}

void
set_active_song_title(FontContext* FntPtr, int win_width, int win_height, SDL_Rect* vp) {
  int y = vp->h * 0.25;

  FntPtr->active->rect.y        = y;
  FntPtr->active->offset_rect.y = y;

  int padding = 0;
  if (FntPtr->active->rect.w >= vp->w) {
    padding = (FntPtr->active->rect.w - vp->w) + 100;
  }

  FntPtr->active->offset_rect.x = (FntPtr->active->rect.x - vp->w) - padding;
}

void
draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->rect);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->offset_rect);
  update_font_rect(&Actve->rect, &Actve->offset_rect, vp->w);
}

void
draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderFillRect(r, &SKPtr->seek_box);
  SDL_RenderFillRect(r, &SKPtr->seek_line);
}

void
draw_vol_bar(SDL_Renderer* r, VolBar* VBar, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderFillRect(r, &VBar->seek_box);
  SDL_RenderFillRect(r, &VBar->seek_line);
}

void
update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max) {
  rect_ptr->x += 1;
  if (rect_ptr->x >= max) {
    swap(&offset_rect->x, &rect_ptr->x);
  }
}

void
swap(int* offset_x, int* x) {
  int tmp    = *x;
  int offtmp = *offset_x;

  *x        = offtmp;
  *offset_x = tmp;
}

/*If I just made my own fonts with pixel art I wouldn't have this abomination. This is my first experience
 * using fonts and honestly I hate it so much. Plus they aren't really fitting to the style I want.*/

void
resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {

  i8  playing_song       = SDLC->SSPtr->pb_state->playing_song;
  i8* song_fonts_created = &FNT->state->song_fonts_created;
  i8* dir_fonts_created  = &FNT->state->dir_fonts_created;
  int win_width          = SDLC->container->win_width;

  TTF_Font** font = &FNT->context_data->font;

  const f32 one_thousandth = 0.016;

  const int MIN_FONT_SIZE = 10;
  const int MAX_FONT_SIZE = 18;

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
  if (*song_fonts_created) {
    *song_fonts_created = FALSE;
    for (int i = 0; i < file_count; i++) {
      FNT->sf_arr[i].font_texture = destroy_texture(FNT->sf_arr[i].font_texture);
    }
    free_ptr(FNT->sf_arr);
    create_song_fonts(FNT, FC->file_state, SDLC->r);
  }

  int dir_count = FC->dir_state->dir_count;
  if (*dir_fonts_created) {
    *dir_fonts_created = FALSE;
    for (int i = 0; i < dir_count; i++) {
      FNT->df_arr[i].font_texture = destroy_texture(FNT->df_arr[i].font_texture);
    }
    free_ptr(FNT->df_arr);
    create_dir_fonts(FNT, FC->dir_state, SDLC->r);
  }

  if (playing_song) {
    create_active_song_font(FNT, FC->file_state, SDLC->r);
  }
}
