
#include "../inc/audio.h"
#include "../inc/music_visualizer.h"

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
