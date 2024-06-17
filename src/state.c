#include "../inc/audio.h"
#include "../inc/font.h"
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
set_viewports(SDLContext* SDLC, SDL_Rect* controls_vp_ptr, SDL_Rect* buttons_vp_ptr, SDL_Rect* viz_vp_ptr) {}

void
song_is_paused(SDLContext* SDLC, FontContext* Fnt) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDL_Rect controls_vp = { 0 };
  SDL_Rect viz_vp      = { 0 };
  SDL_Rect settings_vp = { 0 };

  set_playing_viewports(SDLC, &controls_vp, &viz_vp, &settings_vp);

  SDLContainer* Cont  = SDLC->container;
  SongState*    SSPtr = SDLC->SSPtr;
  SeekBar*      SkBar = SDLC->SSPtr->seek_bar;
  VolBar*       VBar  = SDLC->SSPtr->vol_bar;
  SDLSprites*   Spr   = SDLC->sprites;

  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;
  i8  vol_latched   = VBar->latched;
  i8  seek_latched  = SkBar->latched;
  int font_ready    = Fnt->active->ready;

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);
    render_set_rgba_sliders(SDLC, &settings_vp);
    render_draw_rgba_sliders(SDLC, &settings_vp);
    break;
  }

  case FALSE: {
    render_set_play_button(SDLC->container, Spr->play_icon, &controls_vp);
    render_set_pause_button(SDLC->container, Spr->pause_icon, &controls_vp);
    render_set_stop_button(SDLC->container, Spr->stop_icon, &controls_vp);

    render_draw_play_button(SDLC->r, Spr->play_icon, &controls_vp);
    render_draw_pause_button(SDLC->r, Spr->pause_icon, &controls_vp);
    render_draw_stop_button(SDLC->r, Spr->stop_icon, &controls_vp);

    if (*buffers_ready) {
      render_bars(SDLC, &viz_vp);
    }

    if (!vol_latched) {
      set_vol_bar(Cont, SSPtr->vol_bar, SSPtr->audio_data, &controls_vp);
    }
    draw_vol_bar(SDLC->r, SSPtr->vol_bar, &controls_vp);

    if (!seek_latched) {
      set_seek_bar(Cont, SkBar, SSPtr->audio_data, &controls_vp);
    }
    draw_seek_bar(SDLC->r, SkBar, &controls_vp);

    if (font_ready) {
      set_active_song_title(Fnt, Cont->win_width, Cont->win_height, &controls_vp);
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
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDL_Rect controls_vp = { 0 };
  SDL_Rect viz_vp      = { 0 };
  SDL_Rect settings_vp = { 0 };

  set_playing_viewports(SDLC, &controls_vp, &viz_vp, &settings_vp);

  SDLContainer*     Cont  = SDLC->container;
  SongState*        SSPtr = SDLC->SSPtr;
  FourierTransform* FTPtr = SDLC->FTPtr;
  SeekBar*          SkBar = SSPtr->seek_bar;
  VolBar*           VBar  = SSPtr->vol_bar;
  SDLSprites*       Spr   = SDLC->sprites;

  i8* buffers_ready = &FTPtr->fft_data->buffers_ready;
  int font_ready    = Fnt->active->ready;

  i8 vol_latched  = VBar->latched;
  i8 seek_latched = SkBar->latched;

  switch (SDLC->viewing_settings) {

  case FALSE: {
    render_set_play_button(SDLC->container, Spr->play_icon, &controls_vp);
    render_set_pause_button(SDLC->container, Spr->pause_icon, &controls_vp);
    render_set_stop_button(SDLC->container, Spr->stop_icon, &controls_vp);

    render_draw_play_button(SDLC->r, Spr->play_icon, &controls_vp);
    render_draw_pause_button(SDLC->r, Spr->pause_icon, &controls_vp);
    render_draw_stop_button(SDLC->r, Spr->stop_icon, &controls_vp);

    if (*buffers_ready) {
      generate_visual(FTPtr->fft_data, FTPtr->fft_buffers, SDLC->spec.freq);
    }

    if (*buffers_ready) {
      render_bars(SDLC, &viz_vp);
    }

    if (!vol_latched) {
      set_vol_bar(Cont, SSPtr->vol_bar, SSPtr->audio_data, &controls_vp);
    }
    draw_vol_bar(SDLC->r, SSPtr->vol_bar, &controls_vp);

    if (!seek_latched) {
      set_seek_bar(Cont, SkBar, SSPtr->audio_data, &controls_vp);
    }
    draw_seek_bar(SDLC->r, SkBar, &controls_vp);

    if (font_ready) {
      set_active_song_title(Fnt, Cont->win_width, Cont->win_height, &controls_vp);
      draw_active_song_title(SDLC->r, Fnt->active, &controls_vp);
    }
    break;
  }

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);
    render_set_rgba_sliders(SDLC, &settings_vp);
    render_draw_rgba_sliders(SDLC, &settings_vp);
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
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDL_Rect dirs_vp     = { 0 };
  SDL_Rect songs_vp    = { 0 };
  SDL_Rect settings_vp = { 0 };

  SDLSprites* Spr = SDLC->sprites;

  set_stopped_viewports(SDLC, &dirs_vp, &songs_vp, &settings_vp);

  i8 dir_fonts_created  = Fnt->state->dir_fonts_created;
  i8 song_fonts_created = Fnt->state->song_fonts_created;

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, Spr->sett_gear);
    render_draw_gear(SDLC->r, Spr->sett_gear);
    render_set_rgba_sliders(SDLC, &settings_vp);
    render_draw_rgba_sliders(SDLC, &settings_vp);
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
