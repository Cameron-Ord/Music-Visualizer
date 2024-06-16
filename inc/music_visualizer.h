#ifndef MUSIC_VISUALIZER_H
#define MUSIC_VISUALIZER_H

#include "macro.h"
#include "types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

struct SDLColours {
  SDL_Color primary;
  SDL_Color secondary;
  SDL_Color tertiary;
  SDL_Rect  viewports[3];
  f32       normalized_positions[3][4];
  int       scaled_positions[3][4];
  SDL_Rect  scroll_bars[3][4];
};

struct SDLContainer {
  int                win_height;
  int                win_width;
  SDL_Rect           dir_viewport;
  SDL_Rect           song_viewport;
  struct SDLColours* theme;
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
  SDL_Rect                 vis_rect;
  i8                       running;
  i8                       viewing_settings;
  struct SettingsGear*     gear_ptr;
  struct SDLContainer*     container;
  struct SDLMouse*         mouse;
  struct FourierTransform* FTPtr;
  struct SongState*        SSPtr;
};

struct AppContext {
  struct SDLContext*       SDLC;
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

char* get_platform_env();
void  clicked_settings_gear(SDLContext* SDLC);
char* get_slash();
int   chmod_dir(char* path, mode_t mode);
int   make_directory(char* path, mode_t mode);
void  clear_files(FileContext* FCPtr);
void  clear_dirs(FileContext* FCPtr);
void  print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev);
void  index_up(FileState* FS);
void  index_down(FileState* FS);
int   create_settings_icon();
void  swap(int* offset_x, int* x);
#endif
