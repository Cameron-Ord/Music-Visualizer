#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/music_visualizer.h"

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
swap_buffers(void* ptr, void* prim, void* sec) {
  if (ptr == NULL) {
    ptr = prim;
    return;
  }

  if (ptr == prim) {
    ptr = sec;
    return;
  }

  if (ptr == sec) {
    ptr = prim;
    return;
  }
}

void
set_visual_buffer(f32* vis_ptr, f32* buf_ptr) {
  if (buf_ptr == NULL) {
    return;
  }

  if (vis_ptr != buf_ptr) {
    vis_ptr = buf_ptr;
  }
}

void
update_audio_position(AudioData* ADta, SeekBar* SKBar, SDL_Rect vp) {
  const int ttl_length       = vp.w * TWO_TENTHS;
  const int current_position = SKBar->seek_box.x;

  f32 normalized = ((float)current_position / (float)ttl_length);
  u32 scaled_pos = normalized * ADta->wav_len;

  if (scaled_pos > ADta->wav_len) {
    return;
  }

  ADta->audio_pos = scaled_pos;
}

void
update_vol_pos(AudioData* ADta, VolBar* VBar, SDL_Rect vp) {
  const int ttl_length       = vp.w * TWO_TENTHS;
  const int current_position = VBar->seek_box.x;

  f32 normalized = ((float)(current_position) / (float)ttl_length);

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

  FTBufsPtr->in_ptr        = FTBufsPtr->fft_in_prim;
  FTBufsPtr->cpy_ptr       = FTBufsPtr->in_cpy_prim;
  FTBufsPtr->pre_ptr       = FTBufsPtr->pre_raw_prim;
  FTBufsPtr->raw_ptr       = FTBufsPtr->out_raw_prim;
  FTBufsPtr->proc_ptr      = FTBufsPtr->processed_prim;
  FTBufsPtr->smoothed_ptr  = FTBufsPtr->smoothed_prim;
  FTBufsPtr->post_ptr      = FTBufsPtr->post_raw_prim;
  FTBufsPtr->visual_buffer = NULL;

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
  if (FS != NULL) {
    printf("\nSTOPPING: %s\n", FS->files[FS->file_index]);
  }

  stop_song(&PBS->playing_song);
  pause_song(FS, &PBS->is_paused, dev_ptr);
  SDL_CloseAudioDevice(*dev_ptr);
}

void
play_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev) {
  if (FS != NULL) {
    printf("\nNOW PLAYING: %d : %s\n", FS->file_index, FS->files[FS->file_index]);
  }

  audio_switch(*dev, 0);
  *is_paused = FALSE;
}

void
pause_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev) {
  if (FS != NULL) {
    printf("\nPAUSING: %s\n", FS->files[FS->file_index]);
  }

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
