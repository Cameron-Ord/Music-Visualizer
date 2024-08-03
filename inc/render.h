#ifndef RENDER_H
#define RENDER_H
#include "macdef.h"
#include "macro.h"
#include "types.h"
#include <SDL2/SDL_render.h>

void  render_set_play_button(PlayIcon* Play, SDL_Rect* vp);
void  render_set_pause_button(PauseIcon* Pause, SDL_Rect* vp);
void  render_set_stop_button(StopIcon* Stop, SDL_Rect* vp);
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
void  draw_seek_bar(SDL_Renderer* r, SDL_Texture* tex, SeekBar* SKPtr, SDL_Rect* vp, SDL_Color* rgba);
void  set_seek_bar(SeekBar* SkBar, AudioData* Aud, SDL_Rect* vp, SDL_Rect* icon_rect);
void  set_vol_bar(VolBar* VBar, AudioData* Aud, SDL_Rect* vp, SDL_Rect* icon_rect);
void  draw_vol_bar(SDL_Renderer* r, SDL_Texture* tex, VolBar* VBar, SDL_Rect* vp, SDL_Color* rgba);
void  update_window_size(SDLContainer* Cont, SDL_Window* w);
void  get_window_container_size(SDL_Window* w, int* width, int* height);
void  clear_render(SDL_Renderer* r);
void  present_render(SDL_Renderer* r);
void  render_background(SDL_Renderer* r, SDL_Color* rgba);
void  render_bars(SDLContext* SDLC, SDL_Rect* vp);
void  set_active_song_title(FontContext* FntPtr, SDL_Rect* vp);
void  draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve, SDL_Rect* vp);
void  update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max);
void  render_set_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count, SDL_Rect* vp);
void  render_draw_dir_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp);
void  render_set_song_list(SDLContext* SDLC, FontContext* FNT, int file_count, SDL_Rect* vp);
void  render_draw_song_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp);
void  set_colour_fonts(SDLContext* SDLC, FontContext* FNT);
void  draw_colour_fonts(SDLContext* SDLC, FontContext* FNT);
#endif
