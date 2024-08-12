#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

void
render_draw_gear(SDL_Renderer* r, SettingsGear* gear) {
  SDL_RenderSetViewport(r, NULL);
  if (gear->tex != NULL) {
    SDL_RenderCopy(r, gear->tex, NULL, &gear->rect);
  }
}

void
render_draw_play_button(SDL_Renderer* r, PlayIcon* Play, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  if (Play->tex != NULL) {
    SDL_RenderCopy(r, Play->tex, NULL, &Play->rect);
  }
}

void
render_draw_pause_button(SDL_Renderer* r, PauseIcon* Pause, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  if (Pause->tex != NULL) {
    SDL_RenderCopy(r, Pause->tex, NULL, &Pause->rect);
  }
}

void
render_draw_stop_button(SDL_Renderer* r, StopIcon* Stop, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  if (Stop->tex != NULL) {
    SDL_RenderCopy(r, Stop->tex, NULL, &Stop->rect);
  }
}

void
draw_active_song_title(SDL_Renderer* r, ActiveSong* Actve, SDL_Rect* vp) {
  SDL_RenderSetViewport(r, vp);
  if (Actve->tex != NULL) {
    SDL_RenderCopy(r, Actve->tex, NULL, &Actve->rect);
    SDL_RenderCopy(r, Actve->tex, NULL, &Actve->offset_rect);
  }
}

void
draw_seek_bar(SDL_Renderer* r, SDL_Texture* tex, SeekBar* SKPtr, SDL_Rect* vp, SDL_Color* rgba) {
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
  if (tex != NULL) {
    SDL_RenderCopy(r, tex, NULL, &SKPtr->seek_box);
  }
}

void
draw_vol_bar(SDL_Renderer* r, SDL_Texture* tex, VolBar* VBar, SDL_Rect* vp, SDL_Color* rgba) {
  SDL_RenderSetViewport(r, vp);
  SDL_SetRenderDrawColor(r, rgba->r, rgba->g, rgba->b, rgba->a);
  if (tex != NULL) {
    SDL_RenderCopy(r, tex, NULL, &VBar->seek_box);
  }
}

void
draw_colour_fonts(SDLContext* SDLC, FontContext* FNT) {
  SDL_RenderSetViewport(SDLC->r, NULL);
  SDL_Color* rgba = &SDLC->container->theme->textbg;
  FontData*  col  = FNT->colours_list;

  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    SDL_Rect* font_rect = &col[i].font_rect;
    SDL_Rect* font_bg   = &col[i].font_bg;

    if (col[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }
    if (col[i].font_texture != NULL) {
      SDL_RenderCopy(SDLC->r, col[i].font_texture, NULL, font_rect);
    }
  }
}

void
render_bars(SDLContext* SDLC, SDL_Rect* vp) {
  int        out_len = SDLC->FTPtr->fft_data->output_len;
  SDL_Color* rgba    = &SDLC->container->theme->tertiary;
  SDL_Color* rgba_s  = &SDLC->container->theme->textbg;

  if (out_len == 0) {
    PRINT_STR_ERR(stderr, "Audio processing returned empty buffer, stopping.. -> current errno : %s",
                  strerror(errno));

    SDLC->SSPtr->pb_state->hard_stop = TRUE;
    stop_playback(NULL, SDLC->SSPtr->pb_state, &SDLC->audio_dev);
    return;
  }

  SDL_RenderSetViewport(SDLC->r, vp);

  f32* smooth_buff = SDLC->FTPtr->fft_buffers->smoothed;
  f32* smear_buff  = SDLC->FTPtr->fft_buffers->smear;
  i16  cell_width  = vp->w / out_len;
  int  h           = vp->h;

  for (int i = 0; i < out_len; ++i) {
    float start = smear_buff[i];
    float end   = smooth_buff[i];

    int      end_x_pos      = (i * (int)(cell_width + cell_width / 2));
    int      end_y_pos      = h - ((float)h * end);
    int      end_bar_height = end * h;
    SDL_Rect sample_end     = { end_x_pos, end_y_pos, cell_width, end_bar_height };

    SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
    SDL_RenderFillRect(SDLC->r, &sample_end);

    int start_x_pos = (i * (int)(cell_width + cell_width / 2));
    int start_y_pos = h - ((float)h * start);

    if (end_y_pos > start_y_pos) {
      SDL_Rect sample_start = { start_x_pos, start_y_pos, cell_width, end_y_pos - start_y_pos };
      SDL_SetRenderDrawColor(SDLC->r, rgba_s->r, rgba_s->g, rgba_s->b, rgba_s->a);
      SDL_RenderFillRect(SDLC->r, &sample_start);
    }
  }
}

void
render_draw_dir_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp) {
  SDL_RenderSetViewport(SDLC->r, vp);

  ListLimiter* LLmtr  = SDLC->container->list_limiter;
  SDL_Color*   rgba   = &SDLC->container->theme->textbg;
  FontData*    df_arr = FNT->df_arr;

  for (size_t i = LLmtr->dir_first_index; i < LLmtr->dir_last_index; i++) {
    SDL_Rect* font_rect = &df_arr[i].font_rect;
    SDL_Rect* font_bg   = &df_arr[i].font_bg;

    if (df_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }

    if (df_arr[i].font_texture != NULL) {
      SDL_RenderCopy(SDLC->r, df_arr[i].font_texture, NULL, font_rect);
    }
  }
}

void
render_draw_song_list(SDLContext* SDLC, FontContext* FNT, SDL_Rect* vp) {
  SDL_RenderSetViewport(SDLC->r, vp);

  ListLimiter* LLmtr  = SDLC->container->list_limiter;
  SDL_Color*   rgba   = &SDLC->container->theme->textbg;
  FontData*    sf_arr = FNT->sf_arr;

  for (size_t i = LLmtr->song_first_index; i < LLmtr->song_last_index; i++) {
    SDL_Rect* font_rect = &sf_arr[i].font_rect;
    SDL_Rect* font_bg   = &sf_arr[i].font_bg;

    if (sf_arr[i].has_bg) {
      SDL_SetRenderDrawColor(SDLC->r, rgba->r, rgba->g, rgba->b, rgba->a);
      SDL_RenderFillRect(SDLC->r, font_bg);
    }

    if (sf_arr[i].font_texture != NULL) {
      SDL_RenderCopy(SDLC->r, sf_arr[i].font_texture, NULL, font_rect);
    }
  }
}
