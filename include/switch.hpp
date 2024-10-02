#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "sdl2defs.hpp"
#include "events.hpp"
#include "audiodefs.hpp"


void goto_next_song(SDL2Wrapper *sdl2_w, StdClassWrapper *std);
void select_song(StdClassWrapper *std, SDL2Wrapper *sdl2_w);
void select_directory(StdClassWrapper *std, SDL2Wrapper *sdl2_w);
void handle_window_event(uint8_t event, StdClassWrapper *std,
                         SDL2Wrapper *sdl2_w);
void directory_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                               SDL2Wrapper *sdl2_w);
void keydown_handle_state(int userstate, SDL_Keysym sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w);
void song_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w);
void playback_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                              SDL2Wrapper *sdl2_w);
void settings_keydown_options(SDL_Keycode sym, uint16_t mod,
                              StdClassWrapper *std, SDL2Wrapper *sdl2_w);

void decrease_win_width(SDL_Window *w);

void increase_win_width(SDL_Window *w);

void increase_win_height(SDL_Window *w);

void decrease_win_height(SDL_Window *w);

#endif