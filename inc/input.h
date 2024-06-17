#ifndef INPUT_H
#define INPUT_H
#include "types.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_types.h>
void  handle_mouse_click(SDLContext* SDLC, FontContext* FNT, FileContext* FC);
void  handle_mouse_motion(SDLContext* SDLC, FontContext* FNT, FileContext* FC);
void  poll_events(AppContext* app);
void  toggle_pause(SDLContext* SDLC, FileState* FS);
void  handle_space_key(SDLContext* SDLC, FontContext* FNT, FileContext* FC);
void  next_song(SDLContext* SDLC, FontContext* FNT, FileContext* FC);
void  prev_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT);
void  random_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT);
void  move_seekbar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, SeekBar* SKBar);
void  move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar);
void  clicked_while_active(SDLContext* SDLC, FileState* FS, const int mouse_x, const int mouse_y);
void  grab_seek_bar(SDLContext* SDLC, FileState* FS, const int mouse_x, const int mouse_y);
void  grab_vol_bar(SDLContext* SDLC, const int mouse_x, const int mouse_y);
void  seek_latch_on(SeekBar* SKBar, int switch_value);
void  vol_latch_on(VolBar* VBar, int switch_value);
char* find_clicked_dir(FontData* df_arr[], int dir_count, const int mouse_arr[]);
int   find_clicked_song(FontData* sf_arr[], int file_count, const int mouse_arr[]);
void  clicked_in_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x,
                      const int mouse_y);
void  clicked_in_dir_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x,
                          const int mouse_y);
void  clicked_in_song_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x,
                           const int mouse_y);
void  start_song_from_menu(SDLContext* SDLC, FileContext* FC, FontContext* FNT);
void  scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, FileContext* FC, char* sign);
void  handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC, FileContext* FC);
void  handle_mouse_release(SDLContext* SDLC, FileState* FS);
int   point_in_rect(int x, int y, SDL_Rect rect);
int   within_bounds_x(int x, int start, int end);
char* check_sign(int num);
#endif
