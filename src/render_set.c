#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

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
render_set_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count, SDL_Rect* vp) {
  ListLimiter* LLmtr = SDLC->container->list_limiter;
  int          width = SDLC->container->win_width;

  const int height_offset = 5;

  size_t last_index = LLmtr->dir_first_index + LLmtr->amount_to_display;
  if (last_index > (size_t)dir_count) {
    last_index = (size_t)dir_count;
  }

  LLmtr->dir_last_index = last_index;

  const f32 increment = 1.0 / (LLmtr->amount_to_display + 1);
  f32       factor    = 1.0 / (LLmtr->amount_to_display + 1);

  FontData* df_arr = FNT->df_arr;

  for (int i = 0; i < dir_count; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;

    /*GOLEM GET YET GONE*/
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
render_set_song_list(SDLContext* SDLC, FontContext* FNT, int file_count, SDL_Rect* vp) {
  ListLimiter* LLmtr = SDLC->container->list_limiter;
  int          width = SDLC->container->win_width;

  const int height_offset = 5;

  size_t last_index = LLmtr->song_first_index + LLmtr->amount_to_display;
  if (last_index > (size_t)file_count) {
    last_index = (size_t)file_count;
  }

  LLmtr->song_last_index = last_index;

  const f32 increment = 1.0 / (LLmtr->amount_to_display + 1);
  f32       factor    = 1.0 / (LLmtr->amount_to_display + 1);

  FontData* sf_arr = FNT->sf_arr;

  for (int i = 0; i < file_count; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;

    // GOLEM GET YE GONE
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
