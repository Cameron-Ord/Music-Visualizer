#include "../inc/audio.h"
#include "../inc/music_visualizer.h"
#include "../inc/utils.h"

void
change_volume(f32* vol, f32 amount) {
  *vol = clamp(*vol, amount, 0.0f, 1.0f);
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
