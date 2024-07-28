#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

int
song_is_over(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  i8  files_exist  = FC->file_state->files_exist;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;
  i8* playing_song = &SDLC->SSPtr->pb_state->playing_song;

  if (files_exist && playing_song) {
    *hard_stop = FALSE;
    index_up(FC->file_state);
    load_song(SDLC, FC, FNT);
  }
  return 0;
}

void
song_is_paused(SDLContext* SDLC, FontContext* Fnt) {

  SDLContainer* Cont  = SDLC->container;
  SongState*    SSPtr = SDLC->SSPtr;
  SeekBar*      SkBar = SDLC->SSPtr->seek_bar;
  SDLSprites*   Spr   = SDLC->sprites;

  SDL_Color* bg_rgba       = &Cont->theme->secondary;
  SDL_Color* rgba          = &Cont->theme->tertiary;
  i8*        buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;
  int        font_ready    = Fnt->active->ready;

  render_background(SDLC->r, bg_rgba);
  clear_render(SDLC->r);

  SDL_Rect controls_vp = { 0 };
  SDL_Rect viz_vp      = { 0 };
  SDL_Rect settings_vp = { 0 };

  set_playing_viewports(SDLC, &controls_vp, &viz_vp, &settings_vp);

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);
    break;
  }

  case FALSE: {
    render_set_play_button(Spr->play_icon, &controls_vp);
    render_set_pause_button(Spr->pause_icon, &controls_vp);
    render_set_stop_button(Spr->stop_icon, &controls_vp);

    render_draw_play_button(SDLC->r, Spr->play_icon, &controls_vp);
    render_draw_pause_button(SDLC->r, Spr->pause_icon, &controls_vp);
    render_draw_stop_button(SDLC->r, Spr->stop_icon, &controls_vp);

    if (*buffers_ready) {
      render_bars(SDLC, &viz_vp);
    }

    set_vol_bar(SSPtr->vol_bar, SSPtr->audio_data, &controls_vp, &Spr->seek_icon->rect_main[0]);
    draw_vol_bar(SDLC->r, Spr->seek_icon->tex, SSPtr->vol_bar, &controls_vp, rgba);

    set_seek_bar(SkBar, SSPtr->audio_data, &controls_vp, &Spr->seek_icon->rect_main[1]);
    draw_seek_bar(SDLC->r, Spr->seek_icon->tex, SkBar, &controls_vp, rgba);

    if (font_ready) {
      set_active_song_title(Fnt, &controls_vp);
      draw_active_song_title(SDLC->r, Fnt->active, &controls_vp);
    }
    break;
  }

  default: {
    break;
  }
  }

  present_render(SDLC->r);
}

void
song_is_playing(SDLContext* SDLC, FontContext* Fnt) {
  SDLContainer*     Cont  = SDLC->container;
  SongState*        SSPtr = SDLC->SSPtr;
  FourierTransform* FTPtr = SDLC->FTPtr;
  SeekBar*          SkBar = SSPtr->seek_bar;
  SDLSprites*       Spr   = SDLC->sprites;

  i8* buffers_ready = &FTPtr->fft_data->buffers_ready;
  int font_ready    = Fnt->active->ready;

  SDL_Color* rgba    = &Cont->theme->tertiary;
  SDL_Color* bg_rgba = &Cont->theme->secondary;

  render_background(SDLC->r, bg_rgba);
  clear_render(SDLC->r);

  SDL_Rect controls_vp = { 0 };
  SDL_Rect viz_vp      = { 0 };
  SDL_Rect settings_vp = { 0 };

  set_playing_viewports(SDLC, &controls_vp, &viz_vp, &settings_vp);

  switch (SDLC->viewing_settings) {

  case FALSE: {
    render_set_play_button(Spr->play_icon, &controls_vp);
    render_set_pause_button(Spr->pause_icon, &controls_vp);
    render_set_stop_button(Spr->stop_icon, &controls_vp);

    render_draw_play_button(SDLC->r, Spr->play_icon, &controls_vp);
    render_draw_pause_button(SDLC->r, Spr->pause_icon, &controls_vp);
    render_draw_stop_button(SDLC->r, Spr->stop_icon, &controls_vp);

    if (*buffers_ready) {
      generate_visual(FTPtr->fft_data, FTPtr->fft_buffers, SDLC->spec.freq);
    }

    if (*buffers_ready) {
      render_bars(SDLC, &viz_vp);
    }

    set_vol_bar(SSPtr->vol_bar, SSPtr->audio_data, &controls_vp, &Spr->seek_icon->rect_main[0]);
    draw_vol_bar(SDLC->r, Spr->seek_icon->tex, SSPtr->vol_bar, &controls_vp, rgba);

    set_seek_bar(SkBar, SSPtr->audio_data, &controls_vp, &Spr->seek_icon->rect_main[1]);
    draw_seek_bar(SDLC->r, Spr->seek_icon->tex, SkBar, &controls_vp, rgba);

    if (font_ready) {
      set_active_song_title(Fnt, &controls_vp);
      draw_active_song_title(SDLC->r, Fnt->active, &controls_vp);
      update_font_rect(&Fnt->active->rect, &Fnt->active->offset_rect, controls_vp.w);
    }
    break;
  }

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);
    break;
  }

  default: {
    break;
  }
  }

  present_render(SDLC->r);
}

void
song_is_stopped(SDLContext* SDLC, FontContext* Fnt, FileContext* FC) {
  i8 dir_fonts_created  = Fnt->state->dir_fonts_created;
  i8 song_fonts_created = Fnt->state->song_fonts_created;

  SDLSprites*   Spr  = SDLC->sprites;
  SDLContainer* Cont = SDLC->container;

  SDL_Color* bg_rgba = &Cont->theme->secondary;

  render_background(SDLC->r, bg_rgba);
  clear_render(SDLC->r);

  SDL_Rect dirs_vp     = { 0 };
  SDL_Rect songs_vp    = { 0 };
  SDL_Rect settings_vp = { 0 };

  set_stopped_viewports(SDLC, &dirs_vp, &songs_vp, &settings_vp);

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);

    set_colour_fonts(SDLC, Fnt);
    draw_colour_fonts(SDLC, Fnt);
    break;
  }

  case FALSE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);

    if (dir_fonts_created) {
      render_set_dir_list(SDLC, Fnt, FC->dir_state->dir_count, &songs_vp);
      render_draw_dir_list(SDLC, Fnt, &dirs_vp);
    }

    if (song_fonts_created) {
      render_set_song_list(SDLC, Fnt, FC->file_state->file_count, &songs_vp);
      render_draw_song_list(SDLC, Fnt, &songs_vp);
    }
    break;
  }

  default: {
    break;
  }
  }

  present_render(SDLC->r);
}

void
handle_state(AppContext* app) {

  SDLContext*  SDLC = app->SDLC;
  FontContext* FNT  = app->FntPtr;
  FileContext* FC   = app->FCPtr;

  i8 playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8 song_ended   = SDLC->SSPtr->pb_state->song_ended;
  i8 is_paused    = SDLC->SSPtr->pb_state->is_paused;

  switch (playing_song) {
  default: {
    break;
  }
  case TRUE: {
    switch (song_ended) {
    default: {
      break;
    }

    case TRUE: {
      song_is_over(SDLC, FNT, FC);
      break;
    }

    case FALSE: {
      switch (is_paused) {
      default: {
        break;
      }

      case TRUE: {
        song_is_paused(SDLC, FNT);
        break;
      }

      case FALSE: {
        song_is_playing(SDLC, FNT);
        break;
      }
      }
      break;
    }
    }
    break;
  }
  case FALSE: {
    song_is_stopped(SDLC, FNT, FC);
    break;
  }
  }
}
