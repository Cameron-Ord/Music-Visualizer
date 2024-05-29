#ifndef MUSIC_VISUALIZER_H
#define MUSIC_VISUALIZER_H
#include "macro.h"
#include "types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct SDLContainer {
  int      win_height;
  int      win_width;
  SDL_Rect dir_viewport;
  SDL_Rect song_viewport;
};

struct SDLMouse {
  int mouse_offset_y;
  int mouse_offset_x;
};

struct SDLContext {
  SDL_Renderer*            r;
  SDL_Window*              w;
  SDL_AudioSpec            spec;
  SDL_AudioDeviceID        audio_dev;
  i8                       running;
  struct SDLContainer*     container;
  struct SDLMouse*         mouse;
  struct FourierTransform* FTPtr;
  struct FileContext*      FCPtr;
  struct FontContext*      FntPtr;
  struct SongState*        SSPtr;
  SDL_Rect                 vis_rect;
};

struct FTransformBuffers {
  f32  fft_in[DOUBLE_N];
  f32  combined_window[N];
  f32c out_raw[N];
  f32  out_log[N];
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
};

struct DirState {
  char** directories;
  int    dir_count;
  int    dir_index;
  i8     dirs_exist;
};

struct FileState {
  char*  selected_dir;
  i8     files_exist;
  char** files;
  int    file_count;
  int    file_index;
};

struct FileContext {
  struct DirState*  dir_state;
  struct FileState* file_state;
};

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

struct Positions {
  int dir_list_pos;
  int dir_list_offset;
  int dir_list_height;

  int song_list_pos;
  int song_list_offset;
  int song_list_height;
};

struct FontData {
  SDL_Texture* font_texture;
  SDL_Surface* font_surface;
  SDL_Rect     font_rect;
  SDL_Rect     font_bg;
  char*        text;
  i32          id;
  i8           has_bg;
};

struct ActiveSong {
  int          x;
  int          y;
  SDL_Rect     rect;
  SDL_Rect     offset_rect;
  SDL_Rect     vp;
  SDL_Texture* tex;
  SDL_Surface* surf;
  char*        text;
  i32          id;
  i8           ready;
};

struct FontState {
  i8  dir_fonts_created;
  i8  song_fonts_created;
  int initialized;
};

struct TTFData {
  TTF_Font* font;
  char*     font_path;
  SDL_Color color;
};

struct FontContext {
  struct TTFData*    context_data;
  struct FontState*  state;
  struct FontData*   df_arr;
  struct FontData*   sf_arr;
  struct Positions*  pos;
  struct ActiveSong* active;
};

typedef enum {
  ExTiny = 6,
  ExxSM  = 8,
  ExSM   = 10,
  SM     = 12,
  MED    = 14,
  LRG    = 16,
  XLRG   = 18,
  XXLRG  = 20,
} FONTSIZE_BINDS;

typedef struct SDLMouse         SDLMouse;
typedef struct SDLContainer     SDLContainer;
typedef struct SDLContext       SDLContext;
typedef struct FourierTransform FourierTransform;
typedef struct FileContext      FileContext;
typedef struct FontContext      FontContext;
typedef struct SongState        SongState;
typedef struct DirState         DirState;
typedef struct TTFData          TTFData;
typedef struct FontState        FontState;

typedef struct FTransformData    FTransformData;
typedef struct AudioData         AudioData;
typedef struct FTransformBuffers FTransformBuffers;
typedef struct PlaybackState     PlaybackState;

typedef struct FileState  FileState;
typedef struct SeekBar    SeekBar;
typedef struct ActiveSong ActiveSong;

typedef struct FontData   FontData;
typedef struct SDLContext SDLContext;
typedef struct Positions  Positions;

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
int               apply_amp(int size, f32c* out_raw, f32* out_proc, f32* smooth_buf);
float             amp(float _Complex z);
void              generate_visual(FourierTransform* FT, SongState* SS);
void              low_pass(float* log_values, int size, float cutoff, int SR);
void              fft_func(float in[], size_t stride, float _Complex out[], size_t n);
void              fft_push(FourierTransform* FT, SongState* SS, int channels, int bytes);
void              create_hann_window(int DS, FourierTransform* FT);
void              baseline_fft_values(FTransformData* data);
void              instantiate_buffers(FTransformBuffers* bufs);

int  music_player(int argc, char** argv);
int  start_daemon(int argc, char** argv);
int  initialize_SDL();
int  create_renderer(SDL_Window** w, SDL_Renderer** r);
int  create_window(SDL_Window** w);
void print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev);

void  zero_buffers(FTransformData* FTData, FTransformBuffers* FTBuf);
void  reset_playback_variables(AudioData* AD, PlaybackState* PBste);
void* free_ptr(void* ptr);

void clear_fonts(FontContext* FntPtr, FileContext* FCPtr);
void clear_files(FontContext* FntPtr, FileContext* FCPtr);
void clear_dirs(FontContext* FntPtr, FileContext* FCPtr);

void         update_viewports(SDLContext* SDLC);
void         get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt);
void         render_dir_list(SDLContext* SDLC);
void         render_song_list(SDLContext* SDLC);
void         clear_render(SDL_Renderer* r);
void         present_render(SDL_Renderer* r);
void         render_background(SDL_Renderer* r);
void         render_bars(SDLContext* SDLC);
SDL_Texture* create_font_texture(SDL_Renderer* r, SDL_Surface* surface);
SDL_Surface* create_font_surface(TTF_Font** font, SDL_Color color, char* text);
void*        destroy_texture(SDL_Texture* tex);
void*        destroy_surface(SDL_Surface* surf);
void         update_font_surface(SDL_Rect* font_rect, int c_width, int c_height);
void         draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr);
void         set_seek_bar(SDLContext* SDLC);
void         set_active_song_title(SDLContext* SDLC);
void         draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve);
void         update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max);

void handle_mouse_motion(SDLContext* SDLC);
void index_up(FileState* FS);
void handle_mouse_click(SDLContext* SDLC);
void index_down(FileState* FS);
void poll_events(SDLContext* SDLC);
void quit(SDLContext* SDLC);
void toggle_pause(SDLContext* SDLC);
void handle_space_key(SDLContext* SDLC);
void next_song(SDLContext* SDLC);
void prev_song(SDLContext* SDLC);
void random_song(SDLContext* SDLC);
void handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC);
void handle_mouse_release(SDLContext* SDLC);

int  fetch_dirs(DirState* dir_state);
int  fetch_files(FileState* file_state);
void setup_dirs();
void baseline_dir_state(DirState* dir);
void baseline_file_state(FileState* file);
void baseline_dir_state(DirState* dir);
void baseline_file_state(FileState* file);

int       point_in_rect(int x, int y, SDL_Rect rect);
void      scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, Sint32 wheel_y);
void      clicked_in_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void      clicked_in_dir_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void      clicked_in_song_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void      reset_songlist_pos(Positions* pos);
void      start_song_from_menu(SDLContext* SDLC);
char*     find_clicked_dir(FontData* df_arr[], int dir_count, const int mouse_arr[]);
int       find_clicked_song(FontData* sf_arr[], int file_count, const int mouse_arr[]);
FontData* get_struct(FontData* arr[], const int mouse_arr[], int len);
void      clicked_while_active(SDLContext* SDLC, int mouse_x, int mouse_y);
void      grab_seek_bar(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void      switch_latch_on(SeekBar* SKBar, int switch_value);
void      move_seekbar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, SeekBar* SKBar);

void clear_existing_list(FontData** sf_arr, int song_fonts_created, FileState* FS, char* selection);
void resize_fonts(SDLContext* SDLC);
int  create_dir_fonts(FontContext* Fnt, DirState* DS, SDL_Renderer* r);
int  create_song_fonts(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
void create_dir_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC);
void create_song_text_bg(const int mouse_x, const int mouse_y, SDLContext* SDLC);
void clear_font_bgs(FontData* arr[], int len);
void baseline_context_data(TTFData* fontcntxt);
void baseline_font_state(FontState* state);
void baseline_pos(Positions* pos);
int  initialize_TTF(TTFData* context_data);
int  open_font(TTFData* context_data);
int  create_active_song_font(FontContext* Fnt, FileState* FS, SDL_Renderer* r);
void clean_text(char text[]);
#endif
