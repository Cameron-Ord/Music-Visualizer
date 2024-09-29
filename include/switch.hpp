#ifndef SWITCH_HPP
#define SWITCH_HPP
#include "defines.hpp"
void goto_next_song(SDL2Wrapper *sdl2_w, StdClassWrapper *std,
                    USERDATA *userdata);
void select_song(StdClassWrapper *std, SDL2Wrapper *sdl2_w);
void select_directory(StdClassWrapper *std, SDL2Wrapper *sdl2_w);
void handle_window_event(std::uint8_t event, StdClassWrapper *std,
                         SDL2Wrapper *sdl2_w);
void directory_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                               SDL2Wrapper *sdl2_w);
void keydown_handle_state(int userstate, SDL_Keysym sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w, USERDATA *userdata);
void song_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w, USERDATA *userdata);
void playback_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                              SDL2Wrapper *sdl2_w);
void settings_keydown_options(SDL_Keycode sym, uint16_t mod,
                              StdClassWrapper *std, SDL2Wrapper *sdl2_w);
#endif