#include "music_visualizer.h"
#include <errno.h>

#include <sndfile.h>
void audio_switch(SDL_AudioDeviceID dev, int status) { SDL_PauseAudioDevice(dev, status); }

void load_song(SDLContext* SDLC) {

  i8  hard_stop    = SDLC->SSPtr->pb_state->hard_stop;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* song_ended   = &SDLC->SSPtr->pb_state->song_ended;

  *song_ended = TRUE;

  FourierTransform* FT = SDLC->FTPtr;

  if (playing_song) {
    stop_playback(SDLC);

    if (hard_stop == TRUE) {
      fprintf(stdout, "Hard stop\n");
      return;
    }
  }

  int err;
  err = read_to_buffer(SDLC);
  if (err < 0) {
    return;
  }

  set_spec_data(SDLC);

  FT->fft_data->buffers_ready = FALSE;
  zero_buffers(FT->fft_data, FT->fft_buffers);
  FT->fft_data->buffers_ready = TRUE;

  create_active_song_font(SDLC->FntPtr, SDLC->FCPtr->file_state, SDLC->r);

  SDLC->audio_dev = create_audio_device(&SDLC->spec);
  if (SDLC->audio_dev < 0) {
    return;
  }

  *song_ended = FALSE;
  print_spec_data(SDLC->spec, SDLC->audio_dev);
  start_song(SDLC->SSPtr);
  play_song(SDLC);
}

void start_song(SongState* SS) { SS->pb_state->playing_song = 1; }

void stop_song(SongState* SS) { SS->pb_state->playing_song = 0; }

void stop_playback(SDLContext* SDLC) {
  FileState* FSPtr = SDLC->FCPtr->file_state;
  printf("\nSTOPPING: %s\n", FSPtr->files[FSPtr->file_index]);
  pause_song(SDLC);
  stop_song(SDLC->SSPtr);
  SDL_CloseAudioDevice(SDLC->audio_dev);
  reset_playback_variables(SDLC->SSPtr->audio_data, SDLC->SSPtr->pb_state);
}

void play_song(SDLContext* SDLC) {
  FileState* FSPtr     = SDLC->FCPtr->file_state;
  i8*        is_paused = &SDLC->SSPtr->pb_state->is_paused;
  printf("\nNOW PLAYING: %d : %s\n", FSPtr->file_index, FSPtr->files[FSPtr->file_index]);
  audio_switch(SDLC->audio_dev, 0);
  *is_paused = FALSE;
}

void pause_song(SDLContext* SDLC) {
  FileState* FSPtr     = SDLC->FCPtr->file_state;
  i8*        is_paused = &SDLC->SSPtr->pb_state->is_paused;
  printf("\nPAUSING: %s\n", FSPtr->files[FSPtr->file_index]);
  audio_switch(SDLC->audio_dev, 1);
  *is_paused = TRUE;
}
