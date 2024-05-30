#include "audio.h"
#include "font.h"
#include "music_visualizer.h"
int song_is_over(SDLContext* SDLC) {
  i8  files_exist = SDLC->FCPtr->file_state->files_exist;
  i8* hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (files_exist) {
    *hard_stop = FALSE;
    index_up(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
  return 0;
}

void song_is_paused(SDLContext* SDLC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  i8 buffers_ready = SDLC->FTPtr->fft_data->buffers_ready;
  if (buffers_ready) {
    render_bars(SDLC);
    i8 latched = SDLC->SSPtr->seek_bar->latched;
    if (!latched) {
      set_seek_bar(SDLC);
    }
    draw_seek_bar(SDLC->r, SDLC->SSPtr->seek_bar);

    int font_ready = SDLC->FntPtr->active->ready;
    if (font_ready) {
      set_active_song_title(SDLC);
      draw_active_song_title(SDLC->r, SDLC->FntPtr->active);
    }
  }
  present_render(SDLC->r);
}

void song_is_playing(SDLContext* SDLC) {
  render_background(SDLC->r);
  clear_render(SDLC->r);

  i8 buffers_ready = SDLC->FTPtr->fft_data->buffers_ready;
  if (buffers_ready) {
    generate_visual(SDLC->FTPtr, SDLC->THPtr);
    render_bars(SDLC);

    i8 latched = SDLC->SSPtr->seek_bar->latched;
    if (!latched) {
      set_seek_bar(SDLC);
    }
    draw_seek_bar(SDLC->r, SDLC->SSPtr->seek_bar);

    int font_ready = SDLC->FntPtr->active->ready;
    if (font_ready) {
      set_active_song_title(SDLC);
      draw_active_song_title(SDLC->r, SDLC->FntPtr->active);
    }
  }
  present_render(SDLC->r);
}

void song_is_stopped(SDLContext* SDLC) {
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

void handle_state(SDLContext* SDLC) {
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

void clear_dirs(FontContext* FntPtr, FileContext* FCPtr) {
  i8  dirs_exist = FCPtr->dir_state->dirs_exist;
  int dir_count  = FCPtr->dir_state->dir_count;

  if (dir_count > 0 && dirs_exist) {
    for (int i = 0; i < dir_count; i++) {
      FCPtr->dir_state->directories[i] = free_ptr(FCPtr->dir_state->directories[i]);
    }
  }

  if (dirs_exist) {
    FCPtr->dir_state->directories = free_ptr(FCPtr->dir_state->directories);
  }
}

void clear_files(FontContext* FntPtr, FileContext* FCPtr) {

  i8  files_exist = FCPtr->file_state->files_exist;
  int file_count  = FCPtr->file_state->file_count;

  if (file_count > 0 && files_exist) {
    for (int i = 0; i < file_count; i++) {
      FCPtr->file_state->files[i] = free_ptr(FCPtr->file_state->files[i]);
    }
  }

  if (files_exist) {
    FCPtr->file_state->files = free_ptr(FCPtr->file_state->files);
  }
}

void clear_fonts(FontContext* FntPtr, FileContext* FCPtr) {

  i8 files_exist        = FCPtr->file_state->files_exist;
  i8 song_fonts_created = FntPtr->state->song_fonts_created;

  if (song_fonts_created && files_exist) {
    int file_count = FCPtr->file_state->file_count;
    for (int i = 0; i < file_count; i++) {
      destroy_texture(FntPtr->sf_arr[i].font_texture);
    }
  }

  i8 dirs_exist        = FCPtr->dir_state->dirs_exist;
  i8 dir_fonts_created = FntPtr->state->dir_fonts_created;

  if (dir_fonts_created && dirs_exist) {
    int dir_count = FCPtr->dir_state->dir_count;
    for (int i = 0; i < dir_count; i++) {
      destroy_texture(FntPtr->df_arr[i].font_texture);
    }
  }

  FntPtr->active->tex  = destroy_texture(FntPtr->active->tex);
  FntPtr->active->text = free_ptr(FntPtr->active->text);
}
