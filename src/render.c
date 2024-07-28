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
render_background(SDL_Renderer* r, SDL_Color* rgba) {
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}

void
present_render(SDL_Renderer* r) {
  SDL_RenderPresent(r);
}

void
update_window_size(SDLContainer* Cont, SDL_Window* w) {
  int* win_width  = &Cont->win_width;
  int* win_height = &Cont->win_height;
  SDL_GetWindowSize(w, win_width, win_height);

  if (*win_height < 150) {
    Cont->list_limiter->amount_to_display = 1;
    return;
  }

  if (*win_height < 300) {
    Cont->list_limiter->amount_to_display = 2;
    return;
  }

  if (*win_height < 500 && *win_height > 300) {
    Cont->list_limiter->amount_to_display = 3;
    return;
  }

  if (*win_height < 800 && *win_height > 500) {
    Cont->list_limiter->amount_to_display = 4;
    return;
  }

  if (*win_height > 800) {
    Cont->list_limiter->amount_to_display = 6;
    return;
  }
}

void
set_colour_fonts(SDLContext* SDLC, FontContext* FNT) {
  const int box_height    = SDLC->container->win_height;
  const int height_offset = 5;
  const f32 increment     = 1.0 / 8;
  f32       factor        = 1.0 / 8;

  FontData* col = FNT->colours_list;

  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    SDL_Rect* font_rect = &col[i].font_rect;
    SDL_Rect* font_bg   = &col[i].font_bg;

    font_rect->y = factor * box_height;
    font_rect->x = 25;

    if (col[i].has_bg) {
      int      bg_y = (factor * box_height) - height_offset;
      SDL_Rect bg   = { font_rect->x - 5, bg_y, font_rect->w + 10, font_rect->h + 10 };

      *font_bg = bg;
    }
    factor += increment;
  }
}

void
draw_colour_fonts(SDLContext* SDLC, FontContext* FNT) {
  SDL_RenderSetViewport(SDLC->r, NULL);
  SDL_Color* rgba = &SDLC->container->theme->textbg;
  FontData*  col  = FNT->colours_list;

  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    SDL_Rect* font_rect = &col[i].font_rect;
    SDL_Rect* font_bg   = &col[i].font_bg;

    if (col[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    SDL_RenderCopy(SDLC->r, col[i].font_texture, NULL, font_rect);
  }
}

void
set_stopped_viewports(SDLContext* SDLC, SDL_Rect* dir_vp_ptr, SDL_Rect* song_vp_ptr,
                      SDL_Rect* settings_vp_ptr) {
  SDLContainer* Cont  = SDLC->container;
  SDLMouse*     Mouse = SDLC->mouse;

  int width  = Cont->win_width;
  int height = Cont->win_height;

  if (width > 1280) {
    SDL_Rect tmp_dir_vp  = { 0, 0, width * HALF, height };
    SDL_Rect tmp_song_vp = { width * HALF, 0, width * HALF, height };
    SDL_Rect tmp_sett    = { 0, 0, width, height };

    *dir_vp_ptr      = tmp_dir_vp;
    *song_vp_ptr     = tmp_song_vp;
    *settings_vp_ptr = tmp_sett;

    Mouse->mouse_offset_y = 0;
    Mouse->mouse_offset_x = width * HALF;

    SDLViewports* Vps = SDLC->container->vps;

    Vps->dir_vp      = tmp_dir_vp;
    Vps->song_vp     = tmp_song_vp;
    Vps->settings_vp = tmp_sett;

    return;
  }

  if (width < 1280) {
    SDL_Rect tmp_dir_vp  = { 0, 0, width * HALF, height * HALF };
    SDL_Rect tmp_song_vp = { 0, height * HALF, width, height * HALF };
    SDL_Rect tmp_sett    = { 0, 0, width, height };

    *dir_vp_ptr      = tmp_dir_vp;
    *song_vp_ptr     = tmp_song_vp;
    *settings_vp_ptr = tmp_sett;

    Mouse->mouse_offset_y = height * HALF;
    Mouse->mouse_offset_x = 0;

    SDLViewports* Vps = SDLC->container->vps;

    Vps->dir_vp      = tmp_dir_vp;
    Vps->song_vp     = tmp_song_vp;
    Vps->settings_vp = tmp_sett;

    return;
  }
}

void
set_playing_viewports(SDLContext* SDLC, SDL_Rect* control_vp_ptr, SDL_Rect* viz_vp_ptr,
                      SDL_Rect* settings_vp_ptr) {
  SDLContainer* Cont = SDLC->container;

  int height = Cont->win_height;
  int width  = Cont->win_width;

  SDL_Rect tmp_cntrl = { 0, 0, width, height * ONE_QUARTER };
  SDL_Rect tmp_viz   = { 0, height * ONE_QUARTER, width, height * THREE_QUARTERS };
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
render_set_gear(SDLContainer* Cont, SettingsGear* gear) {
  const int w = 32;
  const int h = 32;

  int x_offset = Cont->win_width - w;

  int padding_x = 10;
  int padding_y = 10;

  gear->rect.w = 32;
  gear->rect.h = 32;

  set_rect(&gear->rect, x_offset - padding_x, 0 + padding_y, w, h);
}

void
render_draw_gear(SDL_Renderer* r, SettingsGear* gear) {
  if (gear->tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, NULL);
  SDL_RenderCopy(r, gear->tex, NULL, &gear->rect);
}

void
render_set_play_button(PlayIcon* Play, SDL_Rect* vp) {
  const int w = 16;
  const int h = 16;

  int y        = vp->h * EIGHT_TENTHS;
  int x_offset = vp->w * FIVE_TENTHS;

  set_rect(&Play->rect, x_offset - (w / 2), y - (h / 2), w, h);
}

void
render_set_pause_button(PauseIcon* Pause, SDL_Rect* vp) {
  const int w = 16;
  const int h = 16;

  int y        = vp->h * EIGHT_TENTHS;
  int x_offset = vp->w * SIX_TENTHS;

  set_rect(&Pause->rect, x_offset - (w / 2), y - (h / 2), w, h);
}

void
render_set_stop_button(StopIcon* Stop, SDL_Rect* vp) {
  const int w = 16;
  const int h = 16;

  int y        = vp->h * EIGHT_TENTHS;
  int x_offset = vp->w * FOUR_TENTHS;

  set_rect(&Stop->rect, x_offset - (w / 2), y - (h / 2), w, h);
}

void
render_draw_play_button(SDL_Renderer* r, PlayIcon* Play, SDL_Rect* vp) {
  if (Play->tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, vp);
  SDL_RenderCopy(r, Play->tex, NULL, &Play->rect);
}

void
render_draw_pause_button(SDL_Renderer* r, PauseIcon* Pause, SDL_Rect* vp) {
  if (Pause->tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, vp);
  SDL_RenderCopy(r, Pause->tex, NULL, &Pause->rect);
}

void
render_draw_stop_button(SDL_Renderer* r, StopIcon* Stop, SDL_Rect* vp) {
  if (Stop->tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, vp);
  SDL_RenderCopy(r, Stop->tex, NULL, &Stop->rect);
}

void
render_bars(SDLContext* SDLC, SDL_Rect* vp) {
  int        out_len = SDLC->FTPtr->fft_data->output_len;
  SDL_Color* rgba    = &SDLC->container->theme->tertiary;

  if (out_len == 0) {
    fprintf(stdout, "OUTPUT LENGTH 0 - SOMETHING HAS GONE HORRIBLY WRONG\n");
    PlaybackState* pb = SDLC->SSPtr->pb_state;

    pb->hard_stop = TRUE;
    stop_playback(NULL, pb, &SDLC->audio_dev);
    return;
  }

  SDL_RenderSetViewport(SDLC->r, vp);

  f32* out = SDLC->FTPtr->fft_buffers->smoothed_ptr;
  if (out == NULL) {
    return;
  }

  i16 cell_width = vp->w / out_len;
  int h          = vp->h;

  for (int i = 0; i < out_len; ++i) {
    float t          = out[i];
    int   x_pos      = (i * (int)(cell_width + cell_width / 2));
    int   y_pos      = h - ((float)h * t);
    int   bar_height = (float)h * t;

    SDL_Rect sample_plus = { x_pos, y_pos, cell_width, bar_height };

    SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
    SDL_RenderFillRect(SDLC->r, &sample_plus);
  }
}

void
render_set_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count, SDL_Rect* vp) {
  ListLimiter* LLmtr = SDLC->container->list_limiter;
  int          width = SDLC->container->win_width;

  const int height_offset = 5;

  size_t last_index = LLmtr->dir_first_index + LLmtr->amount_to_display;
  if (last_index > (size_t)dir_count - 1) {
    last_index = (size_t)dir_count - 1;
  }

  LLmtr->dir_last_index = last_index;

  const f32 increment = 1.0 / (LLmtr->amount_to_display + 1);
  f32       factor    = 1.0 / (LLmtr->amount_to_display + 1);

  FontData* df_arr = FNT->df_arr;

  for (int i = 0; i < dir_count; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;

    /*GOLEM GET YET GONE*/
    /*I want to change this to actual logic in the future. It will be simple to do this, as the problem lies
     * with my point_in_rect function. Since the rectangles for all the items here technically have a position
     * regardless of them being shown or not, when the mouse clicks, the hitbox is still the unshown rect (x
     * is 0, y is 0. So top left of the window), so I need to add a condition to the visible items and add a
     * check using that variable. or I can just keep this method as they need to be out of the way for the
     * displayed fonts*/
    font_rect->y = 0;
    font_rect->x = (width + width);
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
  SDL_Color*   rgba   = &SDLC->container->theme->textbg;
  FontData*    df_arr = FNT->df_arr;

  for (size_t i = LLmtr->dir_first_index; i < LLmtr->dir_last_index; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;
    SDL_Rect* font_bg   = &df_arr[i].font_bg;

    if (df_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    SDL_RenderCopy(SDLC->r, df_arr[i].font_texture, NULL, font_rect);
  }
}

void
render_set_song_list(SDLContext* SDLC, FontContext* FNT, int file_count, SDL_Rect* vp) {
  ListLimiter* LLmtr = SDLC->container->list_limiter;
  int          width = SDLC->container->win_width;

  const int height_offset = 5;

  size_t last_index = LLmtr->song_first_index + LLmtr->amount_to_display;
  if (last_index > (size_t)file_count - 1) {
    last_index = (size_t)file_count - 1;
  }

  LLmtr->song_last_index = last_index;

  const f32 increment = 1.0 / (LLmtr->amount_to_display + 1);
  f32       factor    = 1.0 / (LLmtr->amount_to_display + 1);

  FontData* sf_arr = FNT->sf_arr;

  for (int i = 0; i < file_count; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;

    // GOLEM GET YE GONE
    /*I want to change this to actual logic in the future. It will be simple to do this, as the problem lies
     * with my point_in_rect function. Since the rectangles for all the items here technically have a position
     * regardless of them being shown or not, when the mouse clicks, the hitbox is still the unshown rect (x
     * is 0, y is 0. So top left of the window), so I need to add a condition to the visible items and add a
     * check using that variable. or I can just keep this method as they need to be out of the way for the
     * displayed fonts*/
    font_rect->y = 0;
    font_rect->x = (width + width);
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
  SDL_Color*   rgba   = &SDLC->container->theme->textbg;
  FontData*    sf_arr = FNT->sf_arr;

  for (size_t i = LLmtr->song_first_index; i < LLmtr->song_last_index; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;
    SDL_Rect* font_bg   = &sf_arr[i].font_bg;

    if (sf_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    SDL_RenderCopy(SDLC->r, sf_arr[i].font_texture, NULL, font_rect);
  }
}

void
set_seek_bar(SeekBar* SkBar, AudioData* Aud, SDL_Rect* vp, SDL_Rect* icon_rect) {
  int ttl_length       = Aud->wav_len;
  int current_position = Aud->audio_pos;

  int pos_x      = vp->w * TWO_TENTHS;
  int sub_amount = (vp->w * TWO_TENTHS) / 2;

  SkBar->normalized_pos = ((float)current_position / (float)ttl_length);
  SkBar->current_pos    = SkBar->normalized_pos * (vp->w * TWO_TENTHS);

  int x = SkBar->current_pos;
  int y = vp->h * SIX_TENTHS;

  int box_size = (icon_rect->w / 2);
  int offset   = pos_x - sub_amount;

  icon_rect->x = x + (offset - box_size);
  icon_rect->y = y - (icon_rect->h / 2);

  SkBar->seek_box = *icon_rect;
}

void
set_vol_bar(VolBar* VBar, AudioData* Aud, SDL_Rect* vp, SDL_Rect* icon_rect) {
  VBar->current_pos = Aud->volume * (vp->w * TWO_TENTHS);

  int pos_x      = vp->w * EIGHT_TENTHS;
  int sub_amount = (vp->w * TWO_TENTHS) / 2;

  int x = VBar->current_pos;
  int y = vp->h * SIX_TENTHS;

  int box_size = (icon_rect->w / 2);
  int offset   = pos_x - sub_amount;

  icon_rect->x = x + (offset - box_size);
  icon_rect->y = y - (icon_rect->h / 2);

  VBar->seek_box = *icon_rect;
}

void
set_active_song_title(FontContext* FntPtr, SDL_Rect* vp) {
  int y = vp->h * TWO_TENTHS;

  FntPtr->active->rect.y        = y;
  FntPtr->active->offset_rect.y = y;

  int padding = 0;
  if (FntPtr->active->rect.w >= vp->w - 100) {
    padding = (FntPtr->active->rect.w - vp->w) + 100;
  }

  FntPtr->active->offset_rect.x = (FntPtr->active->rect.x - vp->w) - padding;
}

void
draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->rect);
  SDL_RenderCopy(r, Actve->tex, NULL, &Actve->offset_rect);
}

void
draw_seek_bar(SDL_Renderer* r, SDL_Texture* tex, SeekBar* SKPtr, SDL_Rect* vp, SDL_Color* rgba) {
  if (tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
  SDL_RenderCopy(r, tex, NULL, &SKPtr->seek_box);
}

void
draw_vol_bar(SDL_Renderer* r, SDL_Texture* tex, VolBar* VBar, SDL_Rect* vp, SDL_Color* rgba) {
  if (tex == NULL) {
    return;
  }
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
  SDL_RenderCopy(r, tex, NULL, &VBar->seek_box);
}

void
update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max) {
  static f32 accumulator;
  f32        increment = 0.5;

  if (accumulator >= 1.0) {
    accumulator = 1.0;
    rect_ptr->x += (int)accumulator;
    if (rect_ptr->x >= max) {
      swap(&offset_rect->x, &rect_ptr->x);
    }
    accumulator = 0.0;
  }

  accumulator += increment;
}

void
swap(int* offset_x, int* x) {
  int tmp    = *x;
  int offtmp = *offset_x;

  *x        = offtmp;
  *offset_x = tmp;
}

void
resize_fonts(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {

  i8  playing_song       = SDLC->SSPtr->pb_state->playing_song;
  i8  song_fonts_created = FNT->state->song_fonts_created;
  i8  dir_fonts_created  = FNT->state->dir_fonts_created;
  i8  col_fonts_created  = FNT->state->col_fonts_created;
  int win_width          = SDLC->container->win_width;

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
  if (song_fonts_created) {
    destroy_song_fonts(FNT, file_count);
    create_song_fonts(FNT, FC->file_state, SDLC->r);
  }

  int dir_count = FC->dir_state->dir_count;
  if (dir_fonts_created) {
    destroy_dir_fonts(FNT, dir_count);
    create_dir_fonts(FNT, FC->dir_state, SDLC->r);
  }

  if (col_fonts_created) {
    destroy_colours_fonts(FNT);
    create_colours_fonts(FNT, SDLC->container->theme->themes, SDLC->r);
  }

  if (playing_song) {
    // this kinda seems confusing, that it lacks the destroy function. It just destroys the existing
    // textures/text inside the function itself before creating new ones. I just didn't seperate that step for
    // this one.
    create_active_song_font(FNT, FC->file_state, SDLC->r);
  }
}
