#ifndef MUSIC_VISUALIZER_H
#define MUSIC_VISUALIZER_H

#include "macro.h"
#include "types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

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
  SDL_Renderer*     r;
  SDL_Window*       w;
  SDL_AudioSpec     spec;
  SDL_AudioDeviceID audio_dev;
  i8                running;
  SDL_Rect          vis_rect;

  struct SDLContainer*     container;
  struct SDLMouse*         mouse;
  struct FourierTransform* FTPtr;
  struct FileContext*      FCPtr;
  struct FontContext*      FntPtr;
  struct SongState*        SSPtr;
};

struct DirState {
  char**            directories;
  int               dir_count;
  int               dir_index;
  i8                dirs_exist;
  struct FileState* FSPtr;
};

struct FileState {
  char*            selected_dir;
  i8               files_exist;
  char**           files;
  int              file_count;
  int              file_index;
  struct DirState* DSPtr;
};

struct FileContext {
  struct DirState*  dir_state;
  struct FileState* file_state;
};

char*     get_platform_env();
char*     get_slash();
int       chmod_dir(char* path, mode_t mode);
int       make_directory(char* path, mode_t mode);
void      clear_files(FileContext* FCPtr);
void      clear_dirs(FileContext* FCPtr);
void      print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev);
void      reset_playback_variables(AudioData* AD, PlaybackState* PBste, FTransformData* FTData);
void*     free_ptr(void* ptr);
void      update_viewports(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w);
void      get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt);
void      render_dir_list(SDLContext* SDLC);
void      render_song_list(SDLContext* SDLC);
void      clear_render(SDL_Renderer* r);
void      present_render(SDL_Renderer* r);
void      render_background(SDL_Renderer* r);
void      render_bars(SDLContext* SDLC);
void*     destroy_texture(SDL_Texture* tex);
void*     destroy_surface(SDL_Surface* surf);
void      draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr);
void      set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud);
void      set_vol_bar(SDLContainer* Cont, VolBar* VBar, AudioData* Aud);
void      draw_vol_bar(SDL_Renderer* r, VolBar* VBar);
void      handle_mouse_motion(SDLContext* SDLC);
void      index_up(FileState* FS);
void      handle_mouse_click(SDLContext* SDLC);
void      index_down(FileState* FS);
void      poll_events(SDLContext* SDLC);
void      toggle_pause(SDLContext* SDLC);
void      handle_space_key(SDLContext* SDLC);
void      next_song(SDLContext* SDLC);
void      prev_song(SDLContext* SDLC);
void      random_song(SDLContext* SDLC);
void      handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC);
void      handle_mouse_release(SDLContext* SDLC);
int       point_in_rect(int x, int y, SDL_Rect rect);
void      scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, Sint32 wheel_y);
int       within_bounds_x(int x, int width);
void      swap(int* offset_x, int* x);
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
void      grab_vol_bar(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void      seek_latch_on(SeekBar* SKBar, int switch_value);
void      vol_latch_on(VolBar* VBar, int switch_value);
void      move_seekbar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, SeekBar* SKBar);
void      move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar);
#endif
