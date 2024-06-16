#ifndef RENDER_H
#define RENDER_H
#include "types.h"
#include <SDL2/SDL_render.h>

void  set_viewports(SDLContext* SDLC, SDL_Rect* control_vp_ptr, SDL_Rect* buttons_vp_ptr,
                    SDL_Rect* viz_vp_ptr);
void  render_set_gear(SDLContainer* Cont, SettingsGear* gear);
void  render_draw_gear(SDL_Renderer* r, SettingsGear* gear);
void* destroy_texture(SDL_Texture* tex);
void* destroy_surface(SDL_Surface* surf);
void  draw_seek_bar(SDL_Renderer* r, SeekBar* SKPtr);
void  set_seek_bar(SDLContainer* Cont, SeekBar* SkBar, AudioData* Aud);
void  set_vol_bar(SDLContainer* Cont, VolBar* VBar, AudioData* Aud);
void  draw_vol_bar(SDL_Renderer* r, VolBar* VBar);
void  update_viewports(SDLContainer* Cont, SDLMouse* Mouse, SDL_Window* w);
void  get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt);
void  render_dir_list(SDLContext* SDLC, FontContext* FNT, int dir_count);
void  render_song_list(SDLContext* SDLC, FontContext* FNT, int file_count);
void  clear_render(SDL_Renderer* r);
void  present_render(SDL_Renderer* r);
void  render_background(SDL_Renderer* r);
void  render_bars(SDLContext* SDLC);
void  render_draw_rgba_sliders(SDLContext* SDLC);
void  render_set_rgba_sliders(SDLContext* SDLC);
#endif
