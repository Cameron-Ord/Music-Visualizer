#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"

void
index_up(FileState* FS) {
  FS->file_index = (FS->file_index + 1) % FS->file_count;
}

void
index_down(FileState* FS) {
  FS->file_index = (FS->file_index - 1 + FS->file_count) % FS->file_count;
}

void
handle_mouse_motion(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  int mouse_x, mouse_y;

  int playing_song      = SDLC->SSPtr->pb_state->playing_song;
  i8  scrollbar_latched = SDLC->SSPtr->seek_bar->latched;
  i8  volbar_latched    = SDLC->SSPtr->vol_bar->latched;

  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (!playing_song) {
    if (FNT->state->dir_fonts_created) {
      create_dir_text_bg(mouse_x, mouse_y, SDLC, FNT, FC);
    }

    if (FNT->state->song_fonts_created && FC->file_state) {
      create_song_text_bg(mouse_x, mouse_y, SDLC, FNT, FC);
    }
  }

  if (SDLC->viewing_settings && FNT->state->col_fonts_created) {
    create_col_text_bg(mouse_x, mouse_y, SDLC, FNT);
  }

  if (playing_song) {
    if (volbar_latched) {
      move_volume_bar(mouse_x, SDLC->container, SDLC->SSPtr->audio_data, SDLC->SSPtr->vol_bar);
    }

    if (scrollbar_latched) {
      move_seekbar(mouse_x, SDLC->container, SDLC->SSPtr->audio_data, SDLC->SSPtr->seek_bar);
    }
  }
}

void
toggle_pause(SDLContext* SDLC, FileState* FS) {
  i8* playing_song = &SDLC->SSPtr->pb_state->playing_song;
  i8* is_paused    = &SDLC->SSPtr->pb_state->is_paused;

  if (*playing_song) {
    switch (*is_paused) {
    case 0: {
      pause_song(FS, is_paused, &SDLC->audio_dev);
      break;
    }
    case 1: {
      play_song(FS, is_paused, &SDLC->audio_dev);
      break;
    }
    }
  }
}

void
handle_space_key(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  i8  files_exist = FC->file_state->files_exist;
  i8* hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (files_exist && !SDLC->viewing_settings) {
    *hard_stop = TRUE;
    load_song(SDLC, FC, FNT);
  }

  if (!files_exist) {
    printf("Cannot play music: No files were found.\n");
  }
}

int
status_check(i8 status[]) {
  if (status[0] && status[1]) {
    return TRUE;
  }

  return FALSE;
}

void
next_song(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  i8  files_exist  = FC->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  i8 status[] = { playing_song, files_exist };

  if (status_check(status) && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    index_up(FC->file_state);
    load_song(SDLC, FC, FNT);
  }
}

void
prev_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  i8  files_exist  = FC->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  i8 status[] = { playing_song, files_exist };

  if (status_check(status) && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    index_down(FC->file_state);
    load_song(SDLC, FC, FNT);
  }
}

void
random_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  int  file_count  = FC->file_state->file_count;
  int* file_index  = &FC->file_state->file_index;
  i8   files_exist = FC->file_state->files_exist;
  i8*  hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (file_count <= 0) return;

  *file_index = rand() % file_count;
  if (files_exist && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    load_song(SDLC, FC, FNT);
  }
}

void
change_volume(f32* vol, f32 amount) {
  *vol = clamp(*vol, amount, 0.0f, 1.0f);
}

f32
clamp(f32 vol, f32 amount, f32 min, f32 max) {
  f32 sum = vol += amount;
  if (sum < min) {
    return min;
  }
  if (sum > max) {
    return max;
  }
  return sum;
}

void
handle_mouse_click(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  clicked_in_rect(SDLC, FNT, FC, mouse_x, mouse_y);
}

char*
check_sign(int num) {
  if (num > 0) {
    return "Positive";
  }

  if (num < 0) {
    return "Negative";
  }

  return "Zero";
}

void
handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC, FileContext* FC) {
  char* sign = check_sign(wheel_y);

  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  const int mouse_arr[] = { mouse_x, mouse_y };

  scroll_in_rect(mouse_arr, SDLC, FC, sign);
}

void
handle_mouse_release(SDLContext* SDLC, FileState* FS) {
  PlaybackState*    PBSte     = SDLC->SSPtr->pb_state;
  SeekBar*          SKBar     = SDLC->SSPtr->seek_bar;
  VolBar*           VBar      = SDLC->SSPtr->vol_bar;
  FourierTransform* FTPtr     = SDLC->FTPtr;
  i8*               is_paused = &PBSte->is_paused;

  if (PBSte->playing_song && SKBar->latched) {
    SKBar->latched = FALSE;
    play_song(FS, is_paused, &SDLC->audio_dev);
    instantiate_buffers(FTPtr->fft_buffers);
  }

  if (PBSte->playing_song && VBar->latched) {
    VBar->latched = FALSE;
  }
}
