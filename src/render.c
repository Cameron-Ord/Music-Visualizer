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
render_set_rgba_sliders(SDLContext* SDLC) {
  SDLColours*   Col  = SDLC->container->theme;
  SDLContainer* Cont = SDLC->container;

  int quart_screen = Cont->win_width * 0.25;
  int vp_height    = Cont->win_height * 0.33;
  int x_offset     = Cont->win_width - quart_screen;

  const size_t len = 3;

  SDL_Rect prim_rend_vp = { x_offset * 0.25, 0, quart_screen, vp_height };
  SDL_Rect secn_rend_vp = { x_offset * 0.25, Cont->win_height * 0.33, quart_screen, vp_height };
  SDL_Rect tert_rend_vp = { x_offset * 0.25, Cont->win_height * 0.66, quart_screen, vp_height };

  SDL_Rect temp_arr[] = { prim_rend_vp, secn_rend_vp, tert_rend_vp };
  for (size_t i = 0; i < len; i++) {
    Col->viewports[i] = temp_arr[i];
  }

  const u8 max = 255;

  SDL_Color* colour_struct_array[] = { &Col->primary, &Col->secondary, &Col->tertiary };

  for (size_t i = 0; i < len; i++) {
    SDL_Color* rgba_field   = colour_struct_array[i];
    u8         rgba_array[] = { rgba_field->r, rgba_field->g, rgba_field->b, rgba_field->a };
    f32        factor       = 0.20;
    for (size_t j = 0; j < len + 1; j++) {
      Col->normalized_positions[i][j] = (float)rgba_array[j] / max;
      Col->scaled_positions[i][j]     = Col->viewports[i].w * ((float)rgba_array[j] / max);

      int      x   = Col->scaled_positions[i][j] - SCROLLBAR_OFFSET;
      SDL_Rect bar = { x, Col->viewports[i].h * factor, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT };

      Col->scroll_bars[i][j] = bar;
      factor += 0.20;
    }
  }
}

void
render_draw_rgba_sliders(SDLContext* SDLC) {
  SDLColours*  Col            = SDLC->container->theme;
  const size_t fields_count   = 4;
  const size_t viewport_count = sizeof(Col->viewports) / sizeof(Col->viewports[0]);

  for (size_t i = 0; i < viewport_count; i++) {
    SDL_RenderSetViewport(SDLC->r, &Col->viewports[i]);
    SDL_SetRenderDrawColor(SDLC->r, Col->primary.r, Col->primary.g, Col->primary.b, Col->primary.a);
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
render_bars(SDLContext* SDLC) {

  int win_width  = SDLC->container->win_width;
  int win_height = SDLC->container->win_height;
  int out_len    = SDLC->FTPtr->fft_data->output_len;

  if (out_len == 0) {
    fprintf(stdout, "OUTPUT LENGTH 0 - SOMETHING HAS GONE HORRIBLY WRONG\n");
    PlaybackState* pb = SDLC->SSPtr->pb_state;

    pb->hard_stop = TRUE;
    stop_playback(NULL, pb, &SDLC->audio_dev);
    return;
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

    SDL_SetRenderDrawColor(SDLC->r, 189, 147, 249, 255);
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
      SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 255);
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
      SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 255);
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

/*I could just move this to a render_set_viewports function and maybe that'd be nicer, but I dont think it
 * matters and this only gets called if the window resizes so it's probably better this way*/

void
update_viewports(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w) {
  int* win_width  = &Cont->win_width;
  int* win_height = &Cont->win_height;

  SDL_GetWindowSize(w, win_width, win_height);

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
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->rect);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->offset_rect);
  update_font_rect(&Actve->rect, &Actve->offset_rect, Actve->vp.w);
}

void
draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr) {
  SDL_RenderSetViewport(r, &SKPtr->vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderFillRect(r, &SKPtr->seek_box);
  SDL_RenderFillRect(r, &SKPtr->seek_line);
}

void
draw_vol_bar(SDL_Renderer* r, VolBar* VBar) {
  SDL_RenderSetViewport(r, &VBar->vp);
  SDL_SetRenderDrawColor(r, 189, 147, 249, 255);
  SDL_RenderFillRect(r, &VBar->seek_box);
  SDL_RenderFillRect(r, &VBar->seek_line);
}

/*If I just made my own fonts with pixel art I wouldn't have this abomination. This is my first experience
 * using fonts and honestly I hate it so much. Plus they aren't really fitting to the style I want.*/

void
resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {

  i8  playing_song       = SDLC->SSPtr->pb_state->playing_song;
  i8* song_fonts_created = &FNT->state->song_fonts_created;
  i8* dir_fonts_created  = &FNT->state->dir_fonts_created;
  int song_vp_w          = SDLC->container->song_viewport.w;
  int dir_vp_w           = SDLC->container->dir_viewport.w;
  int win_width          = (dir_vp_w < song_vp_w) ? song_vp_w : dir_vp_w;

  TTF_Font** font = &FNT->context_data->font;

  const f32 one_thousandth = 0.024;

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
