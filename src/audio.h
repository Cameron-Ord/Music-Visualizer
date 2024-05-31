#ifndef AUDIO_H
#define AUDIO_H
#include "macro.h"
#include "types.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_render.h>
typedef enum {
  MAX_BUFFER_DS = (1 << 5),
  MIN_BUFFER_DS = (1 << 2),
  DEFAULT_DS    = (1 << 2)
} DOWNSAMPLING_BINDS;

struct AudioData {
  f32* buffer;
  u32  wav_len;
  u32  audio_pos;
  i8   channels;
  int  sr;
  int  format;
  int  samples;
  int  total_bytes;
};

struct PlaybackState {
  i8   is_paused;
  i8   playing_song;
  char currently_playing;
  i8   hard_stop;
  i8   song_ended;
  i8   is_ready;
};

struct SeekBar {
  f32      normalized_pos;
  int      current_pos;
  SDL_Rect seek_box;
  SDL_Rect seek_line;
  SDL_Rect vp;
  int      latched;
};

struct SongState {
  struct PlaybackState* pb_state;
  struct SeekBar*       seek_bar;
  struct AudioData*     audio_data;
};

struct FTransformBuffers {
  f32  fft_in[DOUBLE_N];
  f32  combined_window[N];
  f32c out_raw[N];
  f32  processed[N / 2];
  f32  smoothed[N / 2];
};

struct FTransformData {
  f32       freq_step;
  int       DS_AMOUNT;
  int       cell_width;
  int       output_len;
  i8        buffers_ready;
  SDL_Rect* rect_buff;
};

struct FourierTransform {
  struct FTransformBuffers* fft_buffers;
  struct FTransformData*    fft_data;
  int                       cpu_cores;
};

void              audio_switch(SDL_AudioDeviceID dev, int status);
void              start_song(SongState* SS);
void              stop_song(SongState* SS);
void              pause_song(SDLContext* SDLC);
void              play_song(SDLContext* SDLC);
void              stop_playback(SDLContext* SDLC);
void              song_is_stopped(SDLContext* SDLC);
void              song_is_playing(SDLContext* SDLC);
void              song_is_paused(SDLContext* SDLC);
int               song_is_over(SDLContext* SDLC);
void              handle_state(SDLContext* SDLC);
void              load_song(SDLContext* SDLC);
int               read_to_buffer(SDLContext* SDLC);
void              set_spec_data(SDLContext* SDLC);
void              callback(void* data, Uint8* stream, int len);
SDL_AudioDeviceID create_audio_device(SDL_AudioSpec* spec);
void              update_audio_position(AudioData* ADta, SeekBar* SKBar);
void              baseline_audio_data(AudioData* data);
void              baseline_seek_bar(SeekBar* skbar);
void              baseline_pb_state(PlaybackState* pbste);
void              apply_amp(int size, FourierTransform* FT, ThreadWrapper* TW);
float             amp(float _Complex z);
void              generate_visual(FourierTransform* FT, ThreadWrapper* TW, int SR);
void              low_pass(float* log_values, int size, float cutoff, int SR);
void              fft_func(float in[], size_t stride, float _Complex out[], size_t n);
void              fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes);
void              create_hann_window(FourierTransform* FT, ThreadWrapper* TW);
void              baseline_fft_values(FTransformData* data);
void              instantiate_buffers(FTransformBuffers* bufs);

#endif
