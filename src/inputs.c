#include "music_visualizer.h"

void index_up(FileState* FS) { FS->file_index = (FS->file_index + 1) % FS->file_count; }

void index_down(FileState* FS) {
  FS->file_index = (FS->file_index - 1 + FS->file_count) % FS->file_count;
}

int increase_ds(FourierTransform* FT) {
  FT->fft_data->buffers_ready = FALSE;

  int ds = FT->fft_data->DS_AMOUNT << 1;
  if (ds < MAX_BUFFER_DS) {
    FT->fft_data->DS_AMOUNT = ds;
  }

  zero_buffers(FT->fft_data, FT->fft_buffers);
  FT->fft_data->buffers_ready = TRUE;
  return 0;
}

int decrease_ds(FourierTransform* FT) {
  FT->fft_data->buffers_ready = FALSE;

  int ds = (FT->fft_data->DS_AMOUNT >> 1);
  if (ds > MIN_BUFFER_DS) {
    FT->fft_data->DS_AMOUNT = ds;
  }

  zero_buffers(FT->fft_data, FT->fft_buffers);
  FT->fft_data->buffers_ready = TRUE;
  return 0;
}

void handle_mouse_motion(SDLContext* SDLC) {
  int          mouse_x, mouse_y;
  FontContext* FS = SDLC->FntPtr;
  FileContext* FC = SDLC->FCPtr;

  int playing_song    = SDLC->SSPtr->pb_state->playing_song;
  int seekbar_latched = SDLC->SSPtr->seek_bar->latched;

  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (!playing_song) {
    if (FS->state->dir_fonts_created) {
      create_dir_text_bg(mouse_x, mouse_y, SDLC);
    }

    if (FS->state->song_fonts_created && FC->file_state != NULL) {
      create_song_text_bg(mouse_x, mouse_y, SDLC);
    }
  } else if (playing_song && seekbar_latched) {
    move_seekbar(mouse_x, SDLC->container, SDLC->SSPtr->audio_data, SDLC->SSPtr->seek_bar);
  }
}

void toggle_pause(SDLContext* SDLC) {
  i8 playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8 is_paused    = SDLC->SSPtr->pb_state->is_paused;

  if (playing_song) {
    switch (is_paused) {
    case 0: {
      pause_song(SDLC);
      break;
    }
    case 1: {
      play_song(SDLC);
      break;
    }
    }
  }
}

void handle_space_key(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  if (files_exist) {
    *hard_stop = TRUE;
    load_song(SDLC);
  } else {
    printf("Cannot play music: No files were found.\n");
  }
}

void next_song(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  if (playing_song && files_exist) {
    *hard_stop = FALSE;
    index_up(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
}

void prev_song(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  if (playing_song && files_exist) {
    *hard_stop = FALSE;
    index_down(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
}

void random_song(SDLContext* SDLC) {
  int  file_count  = SDLC->FCPtr->file_state->file_count;
  int* file_index  = &SDLC->FCPtr->file_state->file_index;
  i8   files_exist = SDLC->FCPtr->file_state->files_exist;
  i8*  hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (file_count <= 0)
    return;

  *file_index = rand() % file_count;
  if (files_exist) {
    *hard_stop = FALSE;
    load_song(SDLC);
    zero_buffers(SDLC->FTPtr->fft_data, SDLC->FTPtr->fft_buffers);
  }
}

void handle_mouse_click(SDLContext* SDLC) {
  int mouse_x, mouse_y, err;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  clicked_in_rect(SDLC, mouse_x, mouse_y);
}

void handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  const int mouse_arr[] = {mouse_x, mouse_y};
  scroll_in_rect(mouse_arr, SDLC, wheel_y);
}

void handle_mouse_release(SDLContext* SDLC) {
  PlaybackState*    PBSte = SDLC->SSPtr->pb_state;
  SeekBar*          SKBar = SDLC->SSPtr->seek_bar;
  FourierTransform* FTPtr = SDLC->FTPtr;

  if (PBSte->playing_song && SKBar->latched) {
    SKBar->latched = FALSE;
    play_song(SDLC);
    zero_buffers(FTPtr->fft_data, FTPtr->fft_buffers);
  }
}
