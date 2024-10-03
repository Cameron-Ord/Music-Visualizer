#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "audio.hpp"
#include "fft.hpp"
#include "files.hpp"
#include "paths.hpp"
#include "theme.hpp"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_video.h>

void goto_next_song(ProgramFiles *files, ProgramPath *pathing, AudioData *ad);
void select_song(ProgramFiles *files, ProgramPath *pathing, AudioData *ad);
void select_directory(ProgramThemes *themes, ProgramFiles *files, ProgramPath *pathing);
void handle_window_event(uint8_t event, ProgramFiles*files, ProgramThemes*themes);
void directory_keydown_options(SDL_Keycode sym, ProgramPath* pathing, ProgramThemes *themes, ProgramFiles *files);
void keydown_handle_state(int userstate, SDL_Keysym sym, ProgramPath *pathing, ProgramFiles *files, AudioData* ad, FourierTransform *fft, ProgramThemes *themes);
void song_keydown_options(SDL_Keycode sym, ProgramFiles *files, AudioData *ad, ProgramPath *pathing);
void playback_keydown_options(SDL_Keycode sym);
void settings_keydown_options(SDL_Keycode sym, uint16_t mod, FourierTransform *fft);

void decrease_win_width(SDL_Window *w);

void increase_win_width(SDL_Window *w);

void increase_win_height(SDL_Window *w);

void decrease_win_height(SDL_Window *w);

#endif