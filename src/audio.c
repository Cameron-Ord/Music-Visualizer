#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/macro.h"
#include "../inc/music_visualizer.h"
#include "../inc/threads.h"

void
callback(void* data, Uint8* stream, int len) {
  SDLContext*       SDLCPtr = (struct SDLContext*)data;
  SongState*        SSPtr   = SDLCPtr->SSPtr;
  FourierTransform* FTPtr   = SDLCPtr->FTPtr;
  AudioData*        Aud     = SSPtr->audio_data;

  u32* wav_len   = &Aud->wav_len;
  u32* audio_pos = &Aud->audio_pos;
  f32* buf       = Aud->buffer;

  u32 remaining = (*wav_len - *audio_pos);

  int samples_to_copy = ((u32)len / sizeof(float) < remaining) ? len / sizeof(float) : remaining;

  float* f32_stream = (float*)stream;

  for (int i = 0; i < samples_to_copy; i++) {
    f32_stream[i] = buf[i + *audio_pos] * Aud->volume;
  }

  // memmove(f32_stream, Aud->buffer + Aud->audio_pos, sizeof(f32) * samples_to_copy);

  if (check_pos(*audio_pos, *wav_len)) {
    fft_push(FTPtr, SSPtr, SDLCPtr->spec.channels, samples_to_copy * sizeof(float));
  }

  *audio_pos += samples_to_copy;

  if (*audio_pos >= *wav_len) {
    fprintf(stdout, "End reached.. Starting next song.\n");
    SSPtr->pb_state->song_ended = TRUE;
  }
}

int
check_pos(u32 audio_pos, u32 len) {
  if (audio_pos > 0 && audio_pos < len) {
    return 1;
  }
  return 0;
}

int
render_await(i8 is_processing) {
  if (!is_processing) {
    return 1;
  }
  return 0;
}

int
read_to_buffer(FileContext* FC, SongState* SS, FourierTransform* FT) {

  char* home = getenv(get_platform_env());
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Err getting home DIR", strerror(errno));
    return -1;
  }

  FileState* FS  = FC->file_state;
  AudioData* Aud = SS->audio_data;
  reset_playback_variables(Aud, SS->pb_state, FT->fft_data);

  char combined_path[PATH_MAX];
  char path[PATH_MAX];

  snprintf(path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s%s", home, get_slash(), get_slash(), get_slash(),
           FS->selected_dir, get_slash());

  sprintf(combined_path, "%s%s", path, FS->files[FS->file_index]);
  printf("READING FILE : %s OF PATH : %s\n", FS->files[FS->file_index], path);

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
#ifdef _WIN32
  printf("--FRAMES :  %lld\n", sfinfo.frames);
#endif

#ifdef __linux__
  printf("--FRAMES :  %ld\n", sfinfo.frames);
#endif

  Aud->samples     = sfinfo.frames * sfinfo.channels;
  Aud->total_bytes = Aud->samples * sizeof(f32);

  Aud->buffer = realloc(Aud->buffer, Aud->samples * sizeof(float));
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

  printf("\n..Done reading. Closing file\n\n");
  sf_close(sndfile);

  return 0;
}

void
update_audio_position(AudioData* ADta, SeekBar* SKBar) {
  int ttl_length       = SKBar->vp.w;
  int current_position = SKBar->seek_box.x + SCROLLBAR_OFFSET;
  if (current_position < 0.0 || current_position > SKBar->vp.w) {
    return;
  }
  f32 normalized  = ((float)current_position / (float)ttl_length);
  int scaled_pos  = normalized * ADta->wav_len;
  ADta->audio_pos = scaled_pos;
}

void
update_vol_pos(AudioData* ADta, VolBar* VBar) {
  int ttl_length       = VBar->vp.w;
  int current_position = VBar->seek_box.x + SCROLLBAR_OFFSET;
  f32 normalized       = ((float)current_position / (float)ttl_length);

  f32 min = 0.0f;
  f32 max = 1.0f;

  if (normalized < min) {
    normalized = min;
  }

  if (normalized > max) {
    normalized = max;
  }

  ADta->volume = normalized;
}

void
print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev) {
  printf("\nFORMAT : %d\n CHANNELS: %d\n FREQ: %d\n USERDATA: %p\n CALLBACK %p\n SAMPLES: %d\n SIZE "
         ": %d\n",
         spec.format, spec.channels, spec.freq, spec.userdata, spec.callback, spec.samples, spec.size);
  printf("\nDEVICE ID : %d\n", dev);
}

void
reset_playback_variables(AudioData* Aud, PlaybackState* PBste, FTransformData* FTData) {
  Aud->audio_pos           = 0;
  Aud->wav_len             = 0;
  PBste->currently_playing = FALSE;
  PBste->playing_song      = FALSE;
  FTData->buffers_ready    = FALSE;
}

void
audio_switch(SDL_AudioDeviceID dev, int status) {
  SDL_PauseAudioDevice(dev, status);
}

void
load_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {

  i8  hard_stop     = SDLC->SSPtr->pb_state->hard_stop;
  i8  playing_song  = SDLC->SSPtr->pb_state->playing_song;
  i8* song_ended    = &SDLC->SSPtr->pb_state->song_ended;
  i8* buffers_ready = &SDLC->FTPtr->fft_data->buffers_ready;

  *song_ended    = FALSE;
  *buffers_ready = FALSE;

  if (playing_song) {
    stop_playback(FC->file_state, SDLC->SSPtr->pb_state, &SDLC->audio_dev);

    if (hard_stop == TRUE) {
      return;
    }
  }

  int err;
  err = read_to_buffer(FC, SDLC->SSPtr, SDLC->FTPtr);
  if (err < 0) {
    return;
  }

  set_spec_data(SDLC);

  create_active_song_font(FNT, FC->file_state, SDLC->r);

  AudioData*         ADPtr     = SDLC->SSPtr->audio_data;
  PlaybackState*     PBPtr     = SDLC->SSPtr->pb_state;
  FTransformData*    FTDataPtr = SDLC->FTPtr->fft_data;
  FTransformBuffers* FTBufsPtr = SDLC->FTPtr->fft_buffers;

  SDLC->audio_dev = create_audio_device(&SDLC->spec);
  if (SDLC->audio_dev == 0) {
    reset_playback_variables(ADPtr, PBPtr, FTDataPtr);
    return;
  }

  print_spec_data(SDLC->spec, SDLC->audio_dev);
  play_song(FC->file_state, &PBPtr->is_paused, &SDLC->audio_dev);
  start_song(&PBPtr->playing_song);
  instantiate_buffers(FTBufsPtr);
  *buffers_ready = TRUE;
}

void
start_song(i8* playing_song) {
  *playing_song = TRUE;
}

void
stop_song(i8* playing_song) {
  *playing_song = FALSE;
}

void
stop_playback(FileState* FS, PlaybackState* PBS, SDL_AudioDeviceID* dev_ptr) {
  printf("\nSTOPPING: %s\n", FS->files[FS->file_index]);

  stop_song(&PBS->playing_song);
  pause_song(FS, &PBS->is_paused, dev_ptr);
  SDL_CloseAudioDevice(*dev_ptr);
}

void
play_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev) {
  printf("\nNOW PLAYING: %d : %s\n", FS->file_index, FS->files[FS->file_index]);

  audio_switch(*dev, 0);
  *is_paused = FALSE;
}

void
pause_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev) {
  printf("\nPAUSING: %s\n", FS->files[FS->file_index]);

  audio_switch(*dev, 1);
  *is_paused = TRUE;
}

void
set_spec_data(SDLContext* SDLC) {
  SDL_AudioSpec* spec = &SDLC->spec;
  AudioData*     ad   = SDLC->SSPtr->audio_data;

  spec->userdata = SDLC;
  spec->callback = callback;
  spec->channels = ad->channels;
  spec->freq     = ad->sr;
  spec->format   = AUDIO_F32;
  spec->samples  = BUFF_SIZE;
}
