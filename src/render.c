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
  SDL_SetRenderDrawColor(r, 40, 42, 54, 0);
}

void
render_draw_gear(SDL_Renderer* r, SettingsGear* gear) {
  SDL_RenderSetViewport(r, NULL);
  SDL_RenderCopy(r, gear->tex, NULL, &gear->rect);
}

void
render_bars(SDLContext* SDLC) {

  int win_width  = SDLC->container->win_width;
  int win_height = SDLC->container->win_height;
  int out_len    = SDLC->FTPtr->fft_data->output_len;

  /*If the out_len is 0 early return, this is extremely unlikely to happen, but if it does : something has
   * gone very wrong. I'm just leaving it here to avoid the division by 0 and have a message for debugging so
   * I know what happened*/
  if (out_len == 0) {
    fprintf(stdout, "OUTPUT LENGTH 0\n");
    exit(EXIT_FAILURE);
  }

  int three_quarters = (int)(win_height * 0.75);
  int nine_tenths    = (int)(win_width * 1.0);
  int offset_diff    = win_width - nine_tenths;

  SDL_Rect viewport = { offset_diff * 0.5, win_height - three_quarters, nine_tenths, three_quarters };
  SDLC->vis_rect    = viewport;

  SDL_RenderSetViewport(SDLC->r, &SDLC->vis_rect);

  f32* out        = SDLC->FTPtr->fft_buffers->smoothed_prim;
  i16  cell_width = viewport.w / out_len;
  int  h          = viewport.h;

  for (int i = 0; i < out_len; ++i) {
    float t          = out[i];
    int   x_pos      = (i * (int)(cell_width + cell_width / 2));
    int   y_pos      = h - ((float)h * t);
    int   bar_height = (float)h * t;

    SDL_Rect sample_plus = { x_pos, y_pos, cell_width, bar_height };

    SDL_SetRenderDrawColor(SDLC->r, 189, 147, 249, 0);
    SDL_RenderFillRect(SDLC->r, &sample_plus);
  }
}

void
render_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count) {
  SDL_RenderSetViewport(SDLC->r, &SDLC->container->dir_viewport);

  Positions* Pos = FNT->pos;

  int y_pos = 0;

  for (int i = 0; i < dir_count; i++) {
    FNT->df_arr[i].font_rect.y = Pos->dir_list_pos + y_pos;
    if (FNT->df_arr[i].has_bg == TRUE) {
      /*offset the y position of the font BG to make it appear centered*/
      FNT->df_arr[i].font_bg.y = (Pos->dir_list_pos - 5) + y_pos;
      SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 0);
      SDL_RenderFillRect(SDLC->r, &FNT->df_arr[i].font_bg);
    }

    y_pos += Y_OFFSET(FNT->context_data->font_size / 2);
    SDL_RenderCopy(SDLC->r, FNT->df_arr[i].font_texture, NULL, &FNT->df_arr[i].font_rect);
  }
}

void
render_song_list(SDLContext* SDLC, FontContext* FNT, int file_count) {
  SDL_RenderSetViewport(SDLC->r, &SDLC->container->song_viewport);

  Positions* Pos = FNT->pos;

  int y_pos = 0;

  for (int i = 0; i < file_count; i++) {
    FNT->sf_arr[i].font_rect.y = Pos->song_list_pos + y_pos;
    if (FNT->sf_arr[i].has_bg == TRUE) {
      /*offset the y position of the font BG to make it appear
       * centered*/
      FNT->sf_arr[i].font_bg.y = (Pos->song_list_pos - 5) + y_pos;
      SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 0);
      SDL_RenderFillRect(SDLC->r, &FNT->sf_arr[i].font_bg);
    }

    y_pos += Y_OFFSET(FNT->context_data->font_size / 2);
    SDL_RenderCopy(SDLC->r, FNT->sf_arr[i].font_texture, NULL, &FNT->sf_arr[i].font_rect);
  }
}

void
present_render(SDL_Renderer* r) {
  SDL_RenderPresent(r);
}

void
update_viewports(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w) {
  int* win_width  = &Cont->win_width;
  int* win_height = &Cont->win_height;

  SDL_GetWindowSize(w, win_width, win_height);

  /*My eyes*/

  if (*win_width > 800) {
    int eighty_percent = (int)(*win_height * 0.8);
    int half           = (int)(*win_width * 0.5);

    int offset_h = *win_height - eighty_percent;
    int offset_w = *win_width - half;

    SDL_Rect LEFT  = { 0, offset_h / 2, offset_w * 0.66, eighty_percent };
    SDL_Rect RIGHT = { offset_w * 0.66, offset_h / 2, half + (half * 0.33), eighty_percent };

    Cont->dir_viewport  = LEFT;
    Cont->song_viewport = RIGHT;

    Mouse->mouse_offset_y = offset_h / 2;
    Mouse->mouse_offset_x = offset_w * 0.66;
  } else if (*win_width < 800) {
    int fourty_percent = (int)(*win_height * 0.4);
    int three_quarters = (int)(*win_width * 0.75);

    int offset_h = *win_height - fourty_percent;

    SDL_Rect LEFT  = { 0, offset_h * 0.1, three_quarters, fourty_percent };
    SDL_Rect RIGHT = { 0, fourty_percent + (offset_h * 0.2), three_quarters, fourty_percent };

    Cont->dir_viewport  = LEFT;
    Cont->song_viewport = RIGHT;

    Mouse->mouse_offset_y = offset_h * 0.1;
    Mouse->mouse_offset_x = 0;
  }
}

void
set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud) {
  int win_width        = Cont->win_width;
  int win_height       = Cont->win_height;
  int ttl_length       = Aud->wav_len;
  int current_position = Aud->audio_pos;

  int one_quarter = (int)(win_height * 0.25);
  int half        = (int)(win_width * 0.25);
  int offset_diff = win_width - half;

  SDL_Rect viewport = { offset_diff * 0.25, 0, half, one_quarter };
  SkBar->vp         = viewport;

  SkBar->normalized_pos = ((float)current_position / (float)ttl_length);
  SkBar->current_pos    = SkBar->normalized_pos * viewport.w;

  int x = SkBar->current_pos - SCROLLBAR_OFFSET;
  int y = viewport.h * 0.75;

  SDL_Rect sk_box  = { x, y, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };
  SDL_Rect sk_line = { 0, y + (SCROLLBAR_HEIGHT_OFFSET), viewport.w, 2 };

  SkBar->seek_box  = sk_box;
  SkBar->seek_line = sk_line;
}

void
set_vol_bar(SDLContainer* Cont, VolBar* VBar, AudioData* Aud) {
  int win_width  = Cont->win_width;
  int win_height = Cont->win_height;

  int one_quarter = (int)(win_height * 0.25);
  int three_quart = (int)(win_width * 0.25);
  int offset_diff = win_width - three_quart;

  SDL_Rect viewport = { offset_diff * 0.75, 0, three_quart, one_quarter };
  VBar->vp          = viewport;
  VBar->current_pos = Aud->volume * viewport.w;

  int x = VBar->current_pos - SCROLLBAR_OFFSET;
  int y = viewport.h * 0.75;

  SDL_Rect sk_box  = { x, y, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };
  SDL_Rect sk_line = { 0, y + (SCROLLBAR_HEIGHT_OFFSET), viewport.w, 2 };

  VBar->seek_box  = sk_box;
  VBar->seek_line = sk_line;
}

void
set_active_song_title(FontContext* FntPtr, int win_width, int win_height) {
  int one_quarter = (int)(win_height * 0.25);
  int half        = (int)(win_width * 0.5);
  int offset_diff = win_width - half;

  SDL_Rect viewport  = { offset_diff * 0.5, 0, half, one_quarter };
  FntPtr->active->vp = viewport;

  int y                         = viewport.h * 0.25;
  FntPtr->active->rect.y        = y;
  FntPtr->active->offset_rect.y = y;

  int padding = 0;
  if (FntPtr->active->rect.w >= viewport.w) {
    padding = (FntPtr->active->rect.w - viewport.w) + 100;
  }
  FntPtr->active->offset_rect.x = (FntPtr->active->rect.x - viewport.w) - padding;
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

void
draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve) {
  SDL_RenderSetViewport(r, &Actve->vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 0);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->rect);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->offset_rect);
  update_font_rect(&Actve->rect, &Actve->offset_rect, Actve->vp.w);
}

void
draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr) {
  SDL_RenderSetViewport(r, &SKPtr->vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 0);
  SDL_RenderFillRect(r, &SKPtr->seek_box);
  SDL_RenderFillRect(r, &SKPtr->seek_line);
}

void
draw_vol_bar(SDL_Renderer* r, VolBar* VBar) {
  SDL_RenderSetViewport(r, &VBar->vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 0);
  SDL_RenderFillRect(r, &VBar->seek_box);
  SDL_RenderFillRect(r, &VBar->seek_line);
}

void
resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {

  i8  playing_song       = SDLC->SSPtr->pb_state->playing_song;
  i8* song_fonts_created = &FNT->state->song_fonts_created;
  i8* dir_fonts_created  = &FNT->state->dir_fonts_created;
  int song_vp_w          = SDLC->container->song_viewport.w;
  int dir_vp_w           = SDLC->container->dir_viewport.w;
  int win_width          = (dir_vp_w < song_vp_w) ? song_vp_w : dir_vp_w;

  TTF_Font** font = &FNT->context_data->font;

  const f32 one_thousandth = 0.016;

  const int MIN_FONT_SIZE = 10;
  const int MAX_FONT_SIZE = 24;

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
