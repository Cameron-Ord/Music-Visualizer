#include "../inc/audio.h"
#include "../inc/font.h"
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
handle_mouse_motion(SDLContext* SDLC) {
  int          mouse_x, mouse_y;
  FontContext* FS = SDLC->FntPtr;
  FileContext* FC = SDLC->FCPtr;

  int playing_song      = SDLC->SSPtr->pb_state->playing_song;
  i8  scrollbar_latched = SDLC->SSPtr->seek_bar->latched;
  i8  volbar_latched    = SDLC->SSPtr->vol_bar->latched;

  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (!playing_song) {
    if (FS->state->dir_fonts_created) {
      create_dir_text_bg(mouse_x, mouse_y, SDLC);
    }

    if (FS->state->song_fonts_created && FC->file_state != NULL) {
      create_song_text_bg(mouse_x, mouse_y, SDLC);
    }
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
toggle_pause(SDLContext* SDLC) {
  i8* playing_song = &SDLC->SSPtr->pb_state->playing_song;
  i8* is_paused    = &SDLC->SSPtr->pb_state->is_paused;

  if (*playing_song) {
    switch (*is_paused) {
    case 0: {
      pause_song(SDLC->FCPtr->file_state, is_paused, &SDLC->audio_dev);
      break;
    }
    case 1: {
      play_song(SDLC->FCPtr->file_state, is_paused, &SDLC->audio_dev);
      break;
    }
    }
  }
}

void
handle_space_key(SDLContext* SDLC) {
  i8  files_exist = SDLC->FCPtr->file_state->files_exist;
  i8* hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (files_exist) {
    *hard_stop = TRUE;
    load_song(SDLC);
  } else {
    printf("Cannot play music: No files were found.\n");
  }
}

void
next_song(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  if (playing_song && files_exist) {
    *hard_stop = FALSE;
    index_up(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
}

void
prev_song(SDLContext* SDLC) {
  i8  files_exist  = SDLC->FCPtr->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  if (playing_song && files_exist) {
    *hard_stop = FALSE;
    index_down(SDLC->FCPtr->file_state);
    load_song(SDLC);
  }
}

void
random_song(SDLContext* SDLC) {
  int  file_count  = SDLC->FCPtr->file_state->file_count;
  int* file_index  = &SDLC->FCPtr->file_state->file_index;
  i8   files_exist = SDLC->FCPtr->file_state->files_exist;
  i8*  hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (file_count <= 0) return;

  *file_index = rand() % file_count;
  if (files_exist) {
    *hard_stop = FALSE;
    load_song(SDLC);
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
handle_mouse_click(SDLContext* SDLC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  clicked_in_rect(SDLC, mouse_x, mouse_y);
}

void
handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  const int mouse_arr[] = { mouse_x, mouse_y };
  scroll_in_rect(mouse_arr, SDLC, wheel_y);
}

void
handle_mouse_release(SDLContext* SDLC) {
  PlaybackState*    PBSte     = SDLC->SSPtr->pb_state;
  SeekBar*          SKBar     = SDLC->SSPtr->seek_bar;
  VolBar*           VBar      = SDLC->SSPtr->vol_bar;
  FourierTransform* FTPtr     = SDLC->FTPtr;
  i8*               is_paused = &SDLC->SSPtr->pb_state->is_paused;

  if (PBSte->playing_song && SKBar->latched) {
    SKBar->latched = FALSE;
    play_song(SDLC->FCPtr->file_state, is_paused, &SDLC->audio_dev);
    instantiate_buffers(FTPtr->fft_buffers);
  }

  if (PBSte->playing_song && VBar->latched) {
    VBar->latched = FALSE;
  }
}
