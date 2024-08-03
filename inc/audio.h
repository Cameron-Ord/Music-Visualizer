#ifndef AUDIO_H
#define AUDIO_H
#include "macdef.h"
#include "macro.h"
#include "types.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_render.h>
#include <complex.h>
#include <math.h>
#include <sndfile.h>

typedef enum { MAX_BUFFER_DS = (1 << 5), MIN_BUFFER_DS = (1 << 2), DEFAULT_DS = (1 << 2) } DOWNSAMPLING_BINDS;

struct AudioData {
  f32*   buffer;
  u32    wav_len;
  u32    audio_pos;
  i8     channels;
  int    sr;
  int    format;
  size_t samples;
  size_t total_bytes;
  f32    volume;
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
  i8       latched;
};

struct VolBar {
  f32      normalized_pos;
  int      current_pos;
  SDL_Rect seek_box;
  SDL_Rect seek_line;
  i8       latched;
};

struct SongState {
  struct PlaybackState*    pb_state;
  struct SeekBar*          seek_bar;
  struct VolBar*           vol_bar;
  struct AudioData*        audio_data;
  struct FourierTransform* FTPtr;
  struct SDLContext*       SDLC;
};

struct FTransformBuffers {
  f32   fft_in_prim[DOUBLE_BUFF];
  f32   fft_in_sec[DOUBLE_BUFF];
  f32   in_cpy_prim[DOUBLE_BUFF];
  f32   in_cpy_sec[DOUBLE_BUFF];
  f32   pre_raw_prim[DOUBLE_BUFF];
  f32   pre_raw_sec[DOUBLE_BUFF];
  f32c  post_raw_prim[DOUBLE_BUFF];
  f32c  post_raw_sec[DOUBLE_BUFF];
  f32c  out_raw_prim[DOUBLE_BUFF];
  f32c  out_raw_sec[DOUBLE_BUFF];
  f32   processed_prim[DOUBLE_BUFF];
  f32   processed_sec[DOUBLE_BUFF];
  f32   smoothed_prim[DOUBLE_BUFF];
  f32   smoothed_sec[DOUBLE_BUFF];
  f32*  in_ptr;
  f32*  cpy_ptr;
  f32*  pre_ptr;
  f32c* raw_ptr;
  f32*  proc_ptr;
  f32*  smoothed_ptr;
  f32c* post_ptr;
  f32*  visual_buffer;
};

struct FTransformData {
  int       cell_width;
  size_t    output_len;
  f32       max_ampl;
  i8        buffers_ready;
  i8        is_processing;
  SDL_Rect* rect_buff;
};

struct FourierTransform {
  struct FTransformBuffers* fft_buffers;
  struct FTransformData*    fft_data;
  struct SDLContext*        SDLC;
  struct SongState*         SSPtr;
};

void              swap_buffers(void* ptr, void* prim, void* sec);
void              audio_switch(SDL_AudioDeviceID dev, int status);
void              start_song(i8* playing_song);
void              stop_song(i8* playing_song);
void              pause_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev);
void              play_song(FileState* FS, i8* is_paused, SDL_AudioDeviceID* dev);
void              stop_playback(FileState* FS, PlaybackState* PBS, SDL_AudioDeviceID* dev_ptr);
void              song_is_stopped(SDLContext* SDLC, FontContext* Fnt, FileContext* FC);
void              song_is_playing(SDLContext* SDLC, FontContext* Fnt);
void              song_is_paused(SDLContext* SDLC, FontContext* Fnt);
int               song_is_over(SDLContext* SDLC, FontContext* FNT, FileContext* FC);
void              handle_state(AppContext* app);
void              load_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT);
int               read_to_buffer(FileContext* FC, SongState* SS, FourierTransform* FT);
void              set_spec_data(SDLContext* SDLC);
void              callback(void* data, Uint8* stream, int len);
SDL_AudioDeviceID create_audio_device(SDL_AudioSpec* spec);
void              update_audio_position(AudioData* ADta, SeekBar* SKBar, SDL_Rect viewport);
void              update_vol_pos(AudioData* ADta, VolBar* VBar, SDL_Rect viewport);
void              change_volume(f32* vol, f32 amount);
void              baseline_pb_state(PlaybackState* pbste);
void              squash_to_log(size_t size, f32c* raw, f32* proc, f32* max_ampl, size_t* len, int SR);
float             amp(float _Complex z);
void              generate_visual(FTransformData* data, FTransformBuffers* bufs, int SR);
void              fft_func(f32* in, size_t stride, f32c* out, size_t n);
void              fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes);
void              hamming_window(f32* in_cpy);
void              instantiate_buffers(FTransformBuffers* bufs);
void              apply_smoothing(size_t len, f32 max_ampl, f32* processed, f32* smoothed);
void              reset_playback_variables(AudioData* AD, PlaybackState* PBste, FTransformData* FTData);
void              set_visual_buffer(f32* vis_ptr, f32* buf_ptr);

#endif
