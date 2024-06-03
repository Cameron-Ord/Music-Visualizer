#include "audio.h"
#include "font.h"
#include "music_visualizer.h"

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
render_bars(SDLContext* SDLC) {
  int win_width  = SDLC->container->win_width;
  int win_height = SDLC->container->win_height;
  int out_len    = SDLC->FTPtr->fft_data->output_len;

  if (out_len == 0) {
    return;
  }

  int three_quarters = (int)(win_height * 0.75);
  int nine_tenths    = (int)(win_width * 1.0);
  int offset_diff    = win_width - nine_tenths;

  SDL_Rect viewport = { offset_diff * 0.5, win_height - three_quarters, nine_tenths, three_quarters };
  SDLC->vis_rect    = viewport;

  SDL_RenderSetViewport(SDLC->r, &SDLC->vis_rect);

  f32* out        = SDLC->FTPtr->fft_buffers->smoothed;
  i16  cell_width = viewport.w / out_len;

  int h = viewport.h;
  int w = viewport.w;

  for (int i = 0; i < out_len; ++i) {
    float t          = out[i];
    int   bar_width  = (int)cell_width;
    int   x_pos      = (i * (int)(cell_width + cell_width / 2));
    int   y_pos      = h - ((float)h * t);
    int   bar_height = (float)h * t;

    SDL_Rect sample_plus = { x_pos, y_pos, bar_width, bar_height };

    SDL_SetRenderDrawColor(SDLC->r, 189, 147, 249, 0);
    SDL_RenderFillRect(SDLC->r, &sample_plus);
  }
}

void
render_dir_list(SDLContext* SDLC) {
  SDL_RenderSetViewport(SDLC->r, &SDLC->container->dir_viewport);

  FontContext* FntPtr = SDLC->FntPtr;
  FileContext* FCPtr  = SDLC->FCPtr;
  Positions*   Pos    = FntPtr->pos;

  int dir_count = FCPtr->dir_state->dir_count;

  int y_pos = 0;

  for (int i = 0; i < dir_count; i++) {
    FntPtr->df_arr[i].font_rect.y = Pos->dir_list_pos + y_pos;
    if (FntPtr->df_arr[i].has_bg == TRUE) {
      /*offset the y position of the font BG to make it appear centered*/
      FntPtr->df_arr[i].font_bg.y = (Pos->dir_list_pos - 5) + y_pos;
      SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 0);
      SDL_RenderFillRect(SDLC->r, &FntPtr->df_arr[i].font_bg);
    }

    y_pos += CONST_YPOS;
    SDL_RenderCopy(SDLC->r, FntPtr->df_arr[i].font_texture, NULL, &FntPtr->df_arr[i].font_rect);
  }
}

void
render_song_list(SDLContext* SDLC) {
  SDL_RenderSetViewport(SDLC->r, &SDLC->container->song_viewport);

  FontContext* FntPtr = SDLC->FntPtr;
  FileContext* FCPtr  = SDLC->FCPtr;
  Positions*   Pos    = FntPtr->pos;

  if (FCPtr->file_state != NULL) {
    int file_count = FCPtr->file_state->file_count;
    int dir_count  = FCPtr->dir_state->dir_count;

    int y_pos = 0;

    for (int i = 0; i < file_count; i++) {
      FntPtr->sf_arr[i].font_rect.y = Pos->song_list_pos + y_pos;
      if (FntPtr->sf_arr[i].has_bg == TRUE) {
        /*offset the y position of the font BG to make it appear
         * centered*/
        FntPtr->sf_arr[i].font_bg.y = (Pos->song_list_pos - 5) + y_pos;
        SDL_SetRenderDrawColor(SDLC->r, 69, 71, 90, 0);
        SDL_RenderFillRect(SDLC->r, &FntPtr->sf_arr[i].font_bg);
      }

      y_pos += CONST_YPOS;
      SDL_RenderCopy(SDLC->r, FntPtr->sf_arr[i].font_texture, NULL, &FntPtr->sf_arr[i].font_rect);
    }
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

  int one_half = (int)(*win_height * 0.5);
  int one_20th = (int)(*win_height * 0.05);

  SDL_Rect LEFT  = { 0, one_20th, *win_width, one_half };
  SDL_Rect RIGHT = { 0, one_half + one_20th, *win_width, one_half };

  Cont->dir_viewport  = LEFT;
  Cont->song_viewport = RIGHT;

  Mouse->mouse_offset_y = one_half + one_20th;
  Mouse->mouse_offset_x = 0;
}

void
set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud) {
  int win_width        = Cont->win_width;
  int win_height       = Cont->win_height;
  int ttl_length       = Aud->wav_len;
  int current_position = Aud->audio_pos;

  int one_quarter = (int)(win_height * 0.25);
  int half        = (int)(win_width * 0.50);
  int offset_diff = win_width - half;

  SDL_Rect viewport = { offset_diff * 0.5, 0, half, one_quarter };
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
resize_fonts(SDLContext* SDLC) {
  FontContext*  FntPtr = SDLC->FntPtr;
  FileContext*  FCPtr  = SDLC->FCPtr;
  SDLContainer* Cont   = SDLC->container;

  i8  playing_song       = SDLC->SSPtr->pb_state->playing_song;
  i8* song_fonts_created = &SDLC->FntPtr->state->song_fonts_created;
  i8* dir_fonts_created  = &SDLC->FntPtr->state->dir_fonts_created;
  int song_vp_w          = SDLC->container->song_viewport.w;
  int dir_vp_w           = SDLC->container->dir_viewport.w;
  int win_width          = (dir_vp_w < song_vp_w) ? song_vp_w : dir_vp_w;

  TTF_Font** font = &FntPtr->context_data->font;
  if (win_width < 580) {
    TTF_SetFontSize(*font, ExSM);
  } else if (win_width < 780 && win_width > 480) {
    TTF_SetFontSize(*font, SM);
  } else if (win_width < 1280 && win_width > 780) {
    TTF_SetFontSize(*font, MED);
  } else if (win_width < 1600 && win_width > 1280) {
    TTF_SetFontSize(*font, LRG);
  } else if (win_width < 1920 && win_width > 1600) {
    TTF_SetFontSize(*font, XLRG);
  } else {
    TTF_SetFontSize(*font, XXLRG);
  }

  int file_count = FCPtr->file_state->file_count;
  if (*song_fonts_created) {
    *song_fonts_created = FALSE;
    for (int i = 0; i < file_count; i++) {
      FntPtr->sf_arr[i].font_texture = destroy_texture(FntPtr->sf_arr[i].font_texture);
    }
    FntPtr->sf_arr = free_ptr(FntPtr->sf_arr);
    create_song_fonts(FntPtr, FCPtr->file_state, SDLC->r);
  }

  int dir_count = FCPtr->dir_state->dir_count;
  if (*dir_fonts_created) {
    *dir_fonts_created = FALSE;
    for (int i = 0; i < dir_count; i++) {
      FntPtr->df_arr[i].font_texture = destroy_texture(FntPtr->df_arr[i].font_texture);
    }
    FntPtr->df_arr = free_ptr(FntPtr->df_arr);
    create_dir_fonts(FntPtr, FCPtr->dir_state, SDLC->r);
  }

  if (playing_song) {
    create_active_song_font(FntPtr, FCPtr->file_state, SDLC->r);
  }
}
