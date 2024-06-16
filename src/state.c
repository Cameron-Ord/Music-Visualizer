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
  SDL_Rect buttons_vp  = { 0 };
  SDL_Rect viz_vp      = { 0 };

  set_viewports(SDLC, &controls_vp, &buttons_vp, &viz_vp);

  SDLContainer* Cont  = SDLC->container;
  SongState*    SSPtr = SDLC->SSPtr;
  SeekBar*      SkBar = SDLC->SSPtr->seek_bar;
  VolBar*       VBar  = SDLC->SSPtr->vol_bar;

  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;
  i8  vol_latched   = VBar->latched;
  i8  seek_latched  = SkBar->latched;
  int font_ready    = Fnt->active->ready;

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);
    render_set_rgba_sliders(SDLC);
    render_draw_rgba_sliders(SDLC);
    break;
  }

  case FALSE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);

    if (*buffers_ready) {
      render_bars(SDLC);
    }

    if (!vol_latched) {
      set_vol_bar(Cont, SSPtr->vol_bar, SSPtr->audio_data);
    }

    draw_vol_bar(SDLC->r, SSPtr->vol_bar);

    if (!seek_latched) {
      set_seek_bar(Cont, SkBar, SSPtr->audio_data);
    }
    draw_seek_bar(SDLC->r, SkBar);

    if (font_ready) {
      set_active_song_title(Fnt, Cont->win_width, Cont->win_height);
      draw_active_song_title(SDLC->r, Fnt->active);
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
  SDL_Rect buttons_vp  = { 0 };
  SDL_Rect viz_vp      = { 0 };

  set_viewports(SDLC, &controls_vp, &buttons_vp, &viz_vp);

  SDLContainer*     Cont  = SDLC->container;
  SongState*        SSPtr = SDLC->SSPtr;
  FourierTransform* FTPtr = SDLC->FTPtr;
  SeekBar*          SkBar = SDLC->SSPtr->seek_bar;

  i8* buffers_ready = &FTPtr->fft_data->buffers_ready;
  int font_ready    = Fnt->active->ready;

  switch (SDLC->viewing_settings) {

  case FALSE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);

    if (*buffers_ready) {
      generate_visual(FTPtr->fft_data, FTPtr->fft_buffers, SDLC->spec.freq);
    }

    if (*buffers_ready) {
      render_bars(SDLC);
    }

    set_vol_bar(Cont, SSPtr->vol_bar, SSPtr->audio_data);
    draw_vol_bar(SDLC->r, SSPtr->vol_bar);

    i8 latched = SkBar->latched;
    if (!latched) {
      set_seek_bar(Cont, SkBar, SSPtr->audio_data);
    }
    draw_seek_bar(SDLC->r, SkBar);

    if (font_ready) {
      set_active_song_title(Fnt, Cont->win_width, Cont->win_height);
      draw_active_song_title(SDLC->r, Fnt->active);
    }
    break;
  }

  case TRUE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);
    render_set_rgba_sliders(SDLC);
    render_draw_rgba_sliders(SDLC);
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

  SDL_Rect controls_vp = { 0 };
  SDL_Rect buttons_vp  = { 0 };
  SDL_Rect viz_vp      = { 0 };

  set_viewports(SDLC, &controls_vp, &buttons_vp, &viz_vp);

  i8 dir_fonts_created  = Fnt->state->dir_fonts_created;
  i8 song_fonts_created = Fnt->state->song_fonts_created;

  switch (SDLC->viewing_settings) {

  case TRUE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);
    render_set_rgba_sliders(SDLC);
    render_draw_rgba_sliders(SDLC);
    break;
  }

  case FALSE: {
    render_set_gear(SDLC->container, SDLC->gear_ptr);
    render_draw_gear(SDLC->r, SDLC->gear_ptr);

    if (dir_fonts_created) {
      render_dir_list(SDLC, Fnt, FC->dir_state->dir_count);
    }

    if (song_fonts_created) {
      render_song_list(SDLC, Fnt, FC->file_state->file_count);
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
