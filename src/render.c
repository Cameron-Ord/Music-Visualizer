#include "../inc/render.h"
#include "../inc/music_visualizer.h"

void
clear_render(SDL_Renderer* r) {
  SDL_RenderClear(r);
}

void
render_background(SDL_Renderer* r, SDL_Color* rgba) {
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
}

void
present_render(SDL_Renderer* r) {
  SDL_RenderPresent(r);
}

void
update_window_size(SDLContainer* Cont, SDL_Window* w) {
  int* win_width  = &Cont->win_width;
  int* win_height = &Cont->win_height;
  SDL_GetWindowSize(w, win_width, win_height);

  if (*win_height < 150) {
    Cont->list_limiter->amount_to_display = 1;
    return;
  }

  if (*win_height < 300) {
    Cont->list_limiter->amount_to_display = 2;
    return;
  }

  if (*win_height < 500 && *win_height > 300) {
    Cont->list_limiter->amount_to_display = 3;
    return;
  }

  if (*win_height < 800 && *win_height > 500) {
    Cont->list_limiter->amount_to_display = 4;
    return;
  }

  if (*win_height > 800) {
    Cont->list_limiter->amount_to_display = 6;
    return;
  }
}

void
update_font_rect(SDL_Rect* rect_ptr, SDL_Rect* offset_rect, int max) {
  static f32 accumulator;
  f32        increment = 0.5;

  if (rect_ptr->x >= max) {
    swap(&offset_rect->x, &rect_ptr->x);
  }

  if (accumulator >= 1.0) {
    accumulator = 1.0;
    rect_ptr->x += (int)accumulator;
    accumulator = 0.0;
  }

  accumulator += increment;
}

void
swap(int* offset_x, int* x) {
  int tmp    = *x;
  int offtmp = *offset_x;

  *x        = offtmp;
  *offset_x = tmp;
}
