#ifndef RENDER_H
#define RENDER_H
#include "types.h"
#include <SDL2/SDL_render.h>

void  render_set_play_button(SDLContainer* Cont, PlayIcon* Play, SDL_Rect* vp);
void  render_set_pause_button(SDLContainer* Cont, PauseIcon* Pause, SDL_Rect* vp);
void  render_set_stop_button(SDLContainer* Cont, StopIcon* Stop, SDL_Rect* vp);
void  render_draw_play_button(SDL_Renderer* r, PlayIcon* Play, SDL_Rect* vp);
void  render_draw_pause_button(SDL_Renderer* r, PauseIcon* Pause, SDL_Rect* vp);
void  render_draw_stop_button(SDL_Renderer* r, StopIcon* Stop, SDL_Rect* vp);
void  set_stopped_viewports(SDLContext* SDLC, SDL_Rect* dir_vp_ptr, SDL_Rect* song_vp_ptr,
                            SDL_Rect* settings_vp_ptr);
void  set_playing_viewports(SDLContext* SDLC, SDL_Rect* control_vp_ptr, SDL_Rect* viz_vp_ptr,
                            SDL_Rect* settings_vp_ptr);
void  render_set_gear(SDLContainer* Cont, SettingsGear* gear);
void  render_draw_gear(SDL_Renderer* r, SettingsGear* gear);
void* destroy_texture(SDL_Texture* tex);
void* destroy_surface(SDL_Surface* surf);
void  draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr, SDL_Rect* vp);
void  set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud, SDL_Rect* vp);
void  set_vol_bar(SDLContainer* Cont, VolBar* VBar, AudioData* Aud, SDL_Rect* vp);
void  draw_vol_bar(SDL_Renderer* r, VolBar* VBar, SDL_Rect* vp);
void  update_window_size(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w);
void  get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt);
void  clear_render(SDL_Renderer* r);
void  present_render(SDL_Renderer* r);
void  render_background(SDL_Renderer* r);
void  render_bars(SDLContext* SDLC, SDL_Rect* vp);
void  set_active_song_title(FontContext* FntPtr, int win_width, int win_height, SDL_Rect* vp);
void  draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve, SDL_Rect* vp);
void  update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max);
void  render_set_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count, SDL_Rect* vp);
void  render_draw_dir_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp);
void  render_set_song_list(SDLContext* SDLC, FontContext* FNT, int file_count, SDL_Rect* vp);
void  render_draw_song_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp);
#endif
