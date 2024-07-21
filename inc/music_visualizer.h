#ifndef MUSIC_VISUALIZER_H
#define MUSIC_VISUALIZER_H

#include "macro.h"
#include "themes.h"
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
  Theme**   themes;
};

struct SDLViewports {
  SDL_Rect dir_vp;
  SDL_Rect song_vp;
  SDL_Rect visualization_vp;
  SDL_Rect controls_vp;
  SDL_Rect settings_vp;
};

struct ListLimiter {
  size_t song_first_index;
  size_t dir_first_index;
  size_t song_last_index;
  size_t dir_last_index;
  size_t amount_to_display;
};

struct SDLContainer {
  int                  win_height;
  int                  win_width;
  struct SDLColours*   theme;
  struct SDLViewports* vps;
  struct ListLimiter*  list_limiter;
};

struct SDLMouse {
  int mouse_offset_y;
  int mouse_offset_x;
};

struct SDLSprites {
  struct SettingsGear* sett_gear;
  struct PlayIcon*     play_icon;
  struct PauseIcon*    pause_icon;
  struct StopIcon*     stop_icon;
  struct SeekIcon*     seek_icon;
};

struct SDLContext {
  SDL_Renderer*            r;
  SDL_Window*              w;
  SDL_AudioSpec            spec;
  SDL_AudioDeviceID        audio_dev;
  i8                       running;
  i8                       viewing_settings;
  struct SDLSprites*       sprites;
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

void  check_size(SDLContainer* Cont, SDL_Window* w);
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
int   update_colours(SDLContext* SDLC);

#endif
