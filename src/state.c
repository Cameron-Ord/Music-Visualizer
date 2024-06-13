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
song_is_paused(SDLContext* SDLC, FontContext* Fnt) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDLContainer* Cont  = SDLC->container;
  SongState*    SSPtr = SDLC->SSPtr;
  SeekBar*      SkBar = SDLC->SSPtr->seek_bar;
  VolBar*       VBar  = SDLC->SSPtr->vol_bar;

  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;

  if (*buffers_ready) {
    render_bars(SDLC);
  }

  i8 vol_latched = VBar->latched;
  if (!vol_latched) {
    set_vol_bar(Cont, SSPtr->vol_bar, SSPtr->audio_data);
  }

  draw_vol_bar(SDLC->r, SSPtr->vol_bar);

  i8 latched = SkBar->latched;
  if (!latched) {
    set_seek_bar(Cont, SkBar, SSPtr->audio_data);
  }
  draw_seek_bar(SDLC->r, SkBar);

  int font_ready = Fnt->active->ready;

  if (font_ready) {
    set_active_song_title(Fnt, Cont->win_width, Cont->win_height);
    draw_active_song_title(SDLC->r, Fnt->active);
  }

  present_render(SDLC->r);
}

void
song_is_playing(SDLContext* SDLC, FontContext* Fnt) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDLContainer*     Cont  = SDLC->container;
  SongState*        SSPtr = SDLC->SSPtr;
  FourierTransform* FTPtr = SDLC->FTPtr;
  SeekBar*          SkBar = SDLC->SSPtr->seek_bar;

  i8* buffers_ready = &FTPtr->fft_data->buffers_ready;

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

  int font_ready = Fnt->active->ready;
  if (font_ready) {
    set_active_song_title(Fnt, Cont->win_width, Cont->win_height);
    draw_active_song_title(SDLC->r, Fnt->active);
  }

  present_render(SDLC->r);
}

void
song_is_stopped(SDLContext* SDLC, FontContext* Fnt, FileContext* FC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  i8 dir_fonts_created  = Fnt->state->dir_fonts_created;
  i8 song_fonts_created = Fnt->state->song_fonts_created;

  if (dir_fonts_created) {
    render_dir_list(SDLC, Fnt, FC->dir_state->dir_count);
  }

  if (song_fonts_created) {
    render_song_list(SDLC, Fnt, FC->file_state->file_count);
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
