#ifndef INIT_H
#define INIT_H

#include "macdef.h"
#include "macro.h"
#include "types.h"
#include <SDL2/SDL.h>

int           initialize_sdl_image();
int           music_player();
int           initialize_SDL();
int           create_renderer(SDL_Window** w, SDL_Renderer** r);
int           create_window(SDL_Window** w);
int           fetch_dirs(DirState* dir_state);
int           fetch_files(FileState* file_state);
PathContainer setup_dirs();
#endif
