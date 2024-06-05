#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/music_visualizer.h"

int
song_is_over(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;
  i8* playing_song = &SDLC->SSPtr->pb_state->playing_song;

  if (files_exist && playing_song) {
    *hard_stop = FALSE;
    index_up(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
  return 0;
}

void
song_is_paused(SDLContext* SDLC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDLContainer* Cont   = SDLC->container;
  FontContext*  FntPtr = SDLC->FntPtr;
  SeekBar*      SkBar  = SDLC->SSPtr->seek_bar;
  VolBar*       VBar   = SDLC->SSPtr->vol_bar;

  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;

  if (*buffers_ready) {
    render_bars(SDLC);
  }

  i8 vol_latched = VBar->latched;
  if (!vol_latched) {
    set_vol_bar(Cont, SDLC->SSPtr->vol_bar, SDLC->SSPtr->audio_data);
  }

  draw_vol_bar(SDLC->r, SDLC->SSPtr->vol_bar);

  i8 latched = SkBar->latched;
  if (!latched) {
    set_seek_bar(Cont, SkBar, SDLC->SSPtr->audio_data);
  }
  draw_seek_bar(SDLC->r, SkBar);

  int font_ready = FntPtr->active->ready;

  if (font_ready) {
    set_active_song_title(FntPtr, Cont->win_width, Cont->win_height);
    draw_active_song_title(SDLC->r, FntPtr->active);
  }

  present_render(SDLC->r);
}

void
song_is_playing(SDLContext* SDLC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  SDLContainer* Cont   = SDLC->container;
  FontContext*  FntPtr = SDLC->FntPtr;
  SeekBar*      SkBar  = SDLC->SSPtr->seek_bar;

  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;
  i8* fft_ready     = &SDLC->FTPtr->fft_data->fft_ready;

  if (*buffers_ready && *fft_ready) {
    generate_visual(SDLC->FTPtr);
    *fft_ready = FALSE;
  }

  if (*buffers_ready && !*fft_ready) {
    render_bars(SDLC);
    *fft_ready = TRUE;
  }

  set_vol_bar(Cont, SDLC->SSPtr->vol_bar, SDLC->SSPtr->audio_data);
  draw_vol_bar(SDLC->r, SDLC->SSPtr->vol_bar);

  i8 latched = SkBar->latched;
  if (!latched) {
    set_seek_bar(Cont, SkBar, SDLC->SSPtr->audio_data);
  }
  draw_seek_bar(SDLC->r, SkBar);

  int font_ready = FntPtr->active->ready;
  if (font_ready) {
    set_active_song_title(FntPtr, Cont->win_width, Cont->win_height);
    draw_active_song_title(SDLC->r, FntPtr->active);
  }

  present_render(SDLC->r);
}

void
song_is_stopped(SDLContext* SDLC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  i8 dir_fonts_created  = SDLC->FntPtr->state->dir_fonts_created;
  i8 song_fonts_created = SDLC->FntPtr->state->song_fonts_created;

  if (dir_fonts_created) {
    render_dir_list(SDLC);
  }

  if (song_fonts_created) {
    render_song_list(SDLC);
  }

  present_render(SDLC->r);
}

void
handle_state(SDLContext* SDLC) {
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
      song_is_over(SDLC);
      break;
    }

    case FALSE: {
      switch (is_paused) {
      default: {
        break;
      }

      case TRUE: {
        song_is_paused(SDLC);
        break;
      }

      case FALSE: {
        song_is_playing(SDLC);
        break;
      }
      }
      break;
    }
    }
    break;
  }
  case FALSE: {
    song_is_stopped(SDLC);
    break;
  }
  }
}
