#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"
#include "../inc/utils.h"

void
grab_seek_bar(SDLContext* SDLC, FileState* FS, const int mouse_x, const int mouse_y) {
  SongState* SSPtr = SDLC->SSPtr;
  SeekBar*   SKBar = SSPtr->seek_bar;

  SDL_Rect hitbox = { SKBar->seek_box.x, SKBar->seek_box.y, SKBar->seek_box.w, SKBar->seek_box.h };

  if (point_in_rect(mouse_x, mouse_y, hitbox)) {
    seek_latch_on(SKBar, TRUE);
    if (SKBar->latched) {
      pause_song(FS, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
    }
  }
}

void
grab_vol_bar(SDLContext* SDLC, const int mouse_x, const int mouse_y) {
  SongState* SSPtr = SDLC->SSPtr;
  VolBar*    VBar  = SSPtr->vol_bar;

  SDL_Rect hitbox = { VBar->seek_box.x, VBar->seek_box.y, VBar->seek_box.w, VBar->seek_box.h };

  if (point_in_rect(mouse_x, mouse_y, hitbox)) {
    vol_latch_on(VBar, TRUE);
  }
}

void
move_seekbar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, SeekBar* SKBar) {
  SDLViewports* Vps         = SDLCntr->vps;
  SDL_Rect      controls_vp = Vps->controls_vp;

  int pos_x      = controls_vp.w * TWO_TENTHS;
  int sub_amount = (controls_vp.w * TWO_TENTHS) / 2;

  int offset = pos_x - sub_amount;

  int start = offset;
  int end   = start + controls_vp.w * TWO_TENTHS;

  if (within_bounds_x(mouse_x, start, end)) {
    SKBar->seek_box.x = mouse_x - offset;
    update_audio_position(ADta, SKBar, Vps->controls_vp);
  }
}

void
move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar) {
  SDLViewports* Vps         = SDLCntr->vps;
  SDL_Rect      controls_vp = Vps->controls_vp;

  int pos_x      = controls_vp.w * EIGHT_TENTHS;
  int sub_amount = (controls_vp.w * TWO_TENTHS) / 2;

  int offset = pos_x - sub_amount;

  int start = offset;
  int end   = start + controls_vp.w * TWO_TENTHS;

  if (within_bounds_x(mouse_x, start, end)) {
    VBar->seek_box.x = mouse_x - offset;
    update_vol_pos(ADta, VBar, Vps->controls_vp);
  }
}

void
toggle_pause(SDLContext* SDLC, FileState* FS) {
  i8* playing_song = &SDLC->SSPtr->pb_state->playing_song;
  i8* is_paused    = &SDLC->SSPtr->pb_state->is_paused;

  if (*playing_song) {
    switch (*is_paused) {
    case 0: {
      pause_song(FS, is_paused, &SDLC->audio_dev);
      break;
    }
    case 1: {
      play_song(FS, is_paused, &SDLC->audio_dev);
      break;
    }
    }
  }
}

void
next_song(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  i8  files_exist  = FC->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  i8 status[] = { playing_song, files_exist };

  if (status_check(status) && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    index_up(FC->file_state);
    load_song(SDLC, FC, FNT);
  }
}

void
prev_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  i8  files_exist  = FC->file_state->files_exist;
  i8  playing_song = SDLC->SSPtr->pb_state->playing_song;
  i8* hard_stop    = &SDLC->SSPtr->pb_state->hard_stop;

  i8 status[] = { playing_song, files_exist };

  if (status_check(status) && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    index_down(FC->file_state);
    load_song(SDLC, FC, FNT);
  }
}

void
random_song(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  int  file_count  = FC->file_state->file_count;
  int* file_index  = &FC->file_state->file_index;
  i8   files_exist = FC->file_state->files_exist;
  i8*  hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (file_count <= 0) return;

  *file_index = rand() % file_count;
  if (files_exist && !SDLC->viewing_settings) {
    *hard_stop = FALSE;
    load_song(SDLC, FC, FNT);
  }
}

void
start_song_from_menu(SDLContext* SDLC, FileContext* FC, FontContext* FNT) {
  SongState* SSPtr = SDLC->SSPtr;

  i8 files_exist  = FC->file_state->files_exist;
  i8 playing_song = SSPtr->pb_state->playing_song;

  /*Determine the outcome based off the state of the application*/

  if (!playing_song && files_exist) {
    load_song(SDLC, FC, FNT);
  } else if (playing_song && files_exist) {
    stop_playback(FC->file_state, SSPtr->pb_state, &SDLC->audio_dev);
  } else {
    printf("Cannot play music: No files were found.\n");
  }
} /*start_song_from_menu*/

void
index_up(FileState* FS) {
  FS->file_index = (FS->file_index + 1) % FS->file_count;
}

void
index_down(FileState* FS) {
  FS->file_index = (FS->file_index - 1 + FS->file_count) % FS->file_count;
}

void
seek_latch_on(SeekBar* SKBar, int switch_value) {
  SKBar->latched = switch_value;
}

void
vol_latch_on(VolBar* VBar, int switch_value) {
  VBar->latched = switch_value;
}

int
update_colours(SDLContext* SDLC, FontContext* Fnt, FileContext* FC, char* theme_name) {
  Theme**     themes_ptr    = SDLC->container->theme->themes;
  SDLColours* current_theme = SDLC->container->theme;

  SDL_Color last_prim = current_theme->primary;
  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    if (themes_ptr[i]->name == theme_name) {
      current_theme->primary   = themes_ptr[i]->prim;
      current_theme->secondary = themes_ptr[i]->secondary;
      current_theme->tertiary  = themes_ptr[i]->tertiary;
      current_theme->text      = themes_ptr[i]->text;
      current_theme->textbg    = themes_ptr[i]->text_bg;
    }
  }

  SDLSprites*   Sprites = SDLC->sprites;
  PlayIcon*     play    = Sprites->play_icon;
  SettingsGear* gear    = Sprites->sett_gear;
  SeekIcon*     seeker  = Sprites->seek_icon;
  StopIcon*     stop    = Sprites->stop_icon;
  PauseIcon*    pause   = Sprites->pause_icon;

  gear->tex = destroy_texture(gear->tex);
  convert_pixel_colours(&gear->surf, last_prim, current_theme->primary);
  gear->tex = create_image_texture(SDLC->r, gear->surf);
  if (gear->tex == NULL) {
    fprintf(stderr, "COULD NOT CREATE TEXTURE FOR ADDR %p\n", gear->surf);
  }

  play->tex = destroy_texture(play->tex);
  convert_pixel_colours(&play->surf, last_prim, current_theme->primary);
  play->tex = create_image_texture(SDLC->r, play->surf);
  if (play->tex == NULL) {
    fprintf(stderr, "COULD NOT CREATE TEXTURE FOR ADDR %p\n", play->surf);
  }

  seeker->tex = destroy_texture(seeker->tex);
  convert_pixel_colours(&seeker->surf, last_prim, current_theme->primary);
  seeker->tex = create_image_texture(SDLC->r, seeker->surf);

  if (seeker->tex == NULL) {
    fprintf(stderr, "COULD NOT CREATE TEXTURE FOR ADDR %p\n", seeker->surf);
  }

  stop->tex = destroy_texture(stop->tex);
  convert_pixel_colours(&stop->surf, last_prim, current_theme->primary);
  stop->tex = create_image_texture(SDLC->r, stop->surf);
  if (stop->tex == NULL) {
    fprintf(stderr, "COULD NOT CREATE TEXTURE FOR ADDR %p\n", stop->surf);
  }

  pause->tex = destroy_texture(pause->tex);
  convert_pixel_colours(&pause->surf, last_prim, current_theme->primary);
  pause->tex = create_image_texture(SDLC->r, pause->surf);
  if (pause->tex == NULL) {
    fprintf(stderr, "COULD NOT CREATE TEXTURE FOR ADDR %p\n", pause->surf);
  }

  int file_count = FC->file_state->file_count;
  int dir_count  = FC->dir_state->dir_count;

  Fnt->context_data->color = current_theme->text;

  destroy_song_fonts(Fnt, file_count);
  create_song_fonts(Fnt, FC->file_state, SDLC->r);

  destroy_dir_fonts(Fnt, dir_count);
  create_dir_fonts(Fnt, FC->dir_state, SDLC->r);

  destroy_colours_fonts(Fnt);
  create_colours_fonts(Fnt, themes_ptr, SDLC->r);

  SDLContainer* Cont = SDLC->container;
  if (Cont->win_icon != NULL) {
    convert_pixel_colours(&Cont->win_icon, last_prim, current_theme->primary);
    SDL_SetWindowIcon(SDLC->w, Cont->win_icon);
  }

  return 0;
}
