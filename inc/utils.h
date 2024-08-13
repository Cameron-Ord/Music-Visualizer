#ifndef UTILS_H
#define UTILS_H
#include "macdef.h"
#include "macro.h"
#include "types.h"
#include <SDL2/SDL_rect.h>

f32   clamp(f32 vol, f32 amount, f32 min, f32 max);
char* check_sign(int num);
int   check_pos(u32 audio_pos, u32 len);
int   status_check(i8 status[]);
int   check_buffer_bounds(int MAX, int MIN, int SIZE);
int   within_bounds_x(int x, int start, int end);
int   point_in_rect(int x, int y, SDL_Rect rect);
#endif
