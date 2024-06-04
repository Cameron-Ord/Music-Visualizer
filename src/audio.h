#ifndef AUDIO_H
#define AUDIO_H
#include "macro.h"
#include "types.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_render.h>
typedef enum { MAX_BUFFER_DS = (1 << 5), MIN_BUFFER_DS = (1 << 2), DEFAULT_DS = (1 << 2) } DOWNSAMPLING_BINDS;

struct AudioData {
  f32* buffer;
  u32  wav_len;
  u32  audio_pos;
  i8   channels;
  int  sr;
  int  format;
  int  samples;
  int  total_bytes;
  f32  volume;
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
  i8       latched;
};

struct VolBar {
  f32      normalized_pos;
  int      current_pos;
  SDL_Rect seek_box;
  SDL_Rect seek_line;
  SDL_Rect vp;
  i8       latched;
};

struct SongState {
  struct PlaybackState*    pb_state;
  struct SeekBar*          seek_bar;
  struct VolBar*           vol_bar;
  struct AudioData*        audio_data;
  struct FileContext*      FCPtr;
  struct FourierTransform* FTPtr;
};

struct FTransformBuffers {
  f32  fft_in[DOUBLE_N];
  f32  in_cpy[N];
  f32c out_raw[N];
  f32  processed[N / 2];
  f32  smoothed[N / 2];
};

struct FTransformData {
  f32       freq_step;
  int       DS_AMOUNT;
  int       cell_width;
  int       output_len;
  f32       max_ampl;
  i8        buffers_ready;
  i8        currently_renderering;
  i8        fft_ready;
  SDL_Rect* rect_buff;
};

struct FourierTransform {
  struct FTransformBuffers* fft_buffers;
  struct FTransformData*    fft_data;
  struct WindowWorker*      winwkr;
  struct FFTWorker*         fftwkr;
};

void              audio_switch(SDL_AudioDeviceID dev, int status);
void              start_song(i8* playing_song);
void              stop_song(i8* playing_song);
void              pause_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev);
void              play_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev);
void              stop_playback(SDLContext* SDLC);
void              song_is_stopped(SDLContext* SDLC);
void              song_is_playing(SDLContext* SDLC);
void              song_is_paused(SDLContext* SDLC);
int               song_is_over(SDLContext* SDLC);
void              handle_state(SDLContext* SDLC);
void              load_song(SDLContext* SDLC);
int               read_to_buffer(FileContext* FC, SongState* SS, FourierTransform* FT);
void              set_spec_data(SDLContext* SDLC);
void              callback(void* data, Uint8* stream, int len);
SDL_AudioDeviceID create_audio_device(SDL_AudioSpec* spec);
void              update_audio_position(AudioData* ADta, SeekBar* SKBar);
void              baseline_audio_data(AudioData* data);
void              baseline_seek_bar(SeekBar* skbar);
void              update_vol_pos(AudioData* ADta, VolBar* VBar);
void              change_volume(f32* vol, f32 amount);
f32               clamp(f32 vol, f32 amount, f32 min, f32 max);
void              baseline_pb_state(PlaybackState* pbste);
void              squash_to_log(int size, FourierTransform* FT);
float             amp(float _Complex z);
void              generate_visual(FourierTransform* FT, int SR);
void              low_pass(float* log_values, int size, float cutoff, int SR);
void              fft_func(float in[], size_t stride, float _Complex out[], size_t n);
void              fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes);
void              create_hann_window(FourierTransform* FT);
void              baseline_fft_values(FTransformData* data);
void              instantiate_buffers(FTransformBuffers* bufs);
int               check_pos(u32 audio_pos, u32 len);
int               render_await(i8 fft_ready);
void              apply_smoothing(int size, FourierTransform* FT);
void              baseline_vol_bar(VolBar* vlbar);

#endif
