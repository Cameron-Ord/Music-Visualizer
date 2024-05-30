#include "audio.h"
#include "font.h"
#include "music_visualizer.h"
#include <sndfile.h>

void callback(void* data, Uint8* stream, int len) {
  SDLContext*       SDLCPtr = (struct SDLContext*)data;
  SongState*        SSPtr   = SDLCPtr->SSPtr;
  FourierTransform* FTPtr   = SDLCPtr->FTPtr;

  AudioData* Aud = SSPtr->audio_data;

  int remaining_samples = (Aud->wav_len - Aud->audio_pos);

  int samples_to_copy =
      (len / sizeof(float) < remaining_samples) ? len / sizeof(float) : remaining_samples;

  float* f32_stream = (float*)stream;

  memcpy(f32_stream, Aud->buffer + Aud->audio_pos, samples_to_copy * sizeof(float));
  if (Aud->audio_pos > 0 && Aud->audio_pos < Aud->wav_len) {
    fft_push(FTPtr, SSPtr, SDLCPtr->spec.channels, samples_to_copy * sizeof(float));
  }

  Aud->audio_pos += samples_to_copy;

  if (Aud->audio_pos >= Aud->wav_len) {
    fprintf(stdout, "\n AUDIO POS: %d, WAV_LEN: %d\n", Aud->audio_pos, Aud->wav_len);
    fprintf(stdout, "End reached.. Starting next song.\n");
    SSPtr->pb_state->song_ended = TRUE;
  }
}

int read_to_buffer(SDLContext* SDLC) {
#ifdef __LINUX__

  char* home = getenv("HOME");
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Err getting home DIR", strerror(errno));
    return -1;
  }

  SongState*   SSPtr = SDLC->SSPtr;
  FileContext* FCPtr = SDLC->FCPtr;

  FileState* FS  = FCPtr->file_state;
  AudioData* Aud = SSPtr->audio_data;

  char combined_path[PATH_MAX];
  char path[PATH_MAX];

  snprintf(path, PATH_MAX, "%s/Music/fftmplayer/%s/", home, FS->selected_dir);
  sprintf(combined_path, "%s%s", path, FS->files[FS->file_index]);
  printf("\nREADING FILE : %s\n\n", FS->files[FS->file_index]);

  SNDFILE* sndfile;
  SF_INFO  sfinfo;

  sndfile = sf_open(combined_path, SFM_READ, &sfinfo);
  if (!sndfile) {
    PRINT_STR_ERR(stderr, "Err opening file for reading", strerror(errno));
    return -1;
  }

  if (sfinfo.channels != 2) {
    fprintf(stderr, "Must be a 2 channel audio file!\n");
    return -1;
  }

  Aud->channels = sfinfo.channels;
  Aud->sr       = sfinfo.samplerate;
  Aud->format   = sfinfo.format;

  printf("--CHANNELS : %d\n", Aud->channels);
  printf("--SAMPLE RATE : %d\n", Aud->sr);
  printf("--FORMAT : %x\n", Aud->format);
  printf("--FRAMES :  %ld\n", sfinfo.frames);

  Aud->samples = sfinfo.frames * sfinfo.channels;
  Aud->buffer  = malloc(Aud->samples * sizeof(float));
  if (Aud->buffer == NULL) {
    return -1;
  }

  memset(Aud->buffer, 0, Aud->samples * sizeof(float));
  printf("--BUFFER : %p\n", Aud->buffer);

  sf_count_t num_read = sf_read_float(sndfile, Aud->buffer, Aud->samples);
  if (num_read < 0) {
    PRINT_STR_ERR(stderr, "Err reading audio data", strerror(errno));
    free_ptr(Aud->buffer);
    sf_close(sndfile);
    return -1;
  }

  Aud->wav_len = Aud->samples;

  FourierTransform* FTPtr = SDLC->FTPtr;

  float DSf                  = (float)N / FTPtr->fft_data->DS_AMOUNT;
  FTPtr->fft_data->freq_step = (float)Aud->sr / DSf;
  printf("\n..Done reading. Closing file\n\n");

  sf_close(sndfile);

  return 0;
#endif

  return -1;
}

void update_audio_position(AudioData* ADta, SeekBar* SKBar) {
  int ttl_length       = SKBar->vp.w;
  int current_position = SKBar->seek_box.x + SCROLLBAR_OFFSET;
  if (current_position < 0.0 || current_position > SKBar->vp.w) {
    return;
  }
  f32 normalized  = ((float)current_position / (float)ttl_length);
  int scaled_pos  = normalized * ADta->wav_len;
  ADta->audio_pos = scaled_pos;
}

void print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev) {
  printf(
      "\nFORMAT : %d\n CHANNELS: %d\n FREQ: %d\n USERDATA: %p\n CALLBACK %p\n SAMPLES: %d\n SIZE "
      ": %d\n",
      spec.format, spec.channels, spec.freq, spec.userdata, spec.callback, spec.samples, spec.size);
  printf("\nDEVICE ID : %d\n", dev);
}

void zero_buffers(FTransformData* FTData, FTransformBuffers* FTBuf) {
  int DS = FTData->DS_AMOUNT;
  memset(FTBuf->fft_in, 0, DOUBLE_N * sizeof(float));
  memset(FTBuf->combined_window, 0, N * sizeof(float));
  memset(FTBuf->out_raw, 0, N * sizeof(float _Complex));
  memset(FTBuf->processed, 0, (N / 2) * sizeof(float));
  memset(FTBuf->smoothed, 0, (N / 2) * sizeof(float));
  memset(FTBuf->out_log, 0, N * sizeof(float));
}

void reset_playback_variables(AudioData* Aud, PlaybackState* PBste) {
  Aud->audio_pos  = 0;
  PBste->is_ready = FALSE;
  Aud->wav_len    = 0;
  Aud->buffer     = free_ptr(Aud->buffer);
}

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
