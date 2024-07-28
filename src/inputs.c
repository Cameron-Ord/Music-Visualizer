#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

void
index_up(FileState* FS) {
  FS->file_index = (FS->file_index + 1) % FS->file_count;
}

void
index_down(FileState* FS) {
  FS->file_index = (FS->file_index - 1 + FS->file_count) % FS->file_count;
}

void
handle_mouse_motion(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  int mouse_x, mouse_y;

  int playing_song      = SDLC->SSPtr->pb_state->playing_song;
  i8  scrollbar_latched = SDLC->SSPtr->seek_bar->latched;
  i8  volbar_latched    = SDLC->SSPtr->vol_bar->latched;

  SDL_GetMouseState(&mouse_x, &mouse_y);
  if (!playing_song) {
    if (FNT->state->dir_fonts_created) {
      create_dir_text_bg(mouse_x, mouse_y, SDLC, FNT, FC);
    }

    if (FNT->state->song_fonts_created && FC->file_state) {
      create_song_text_bg(mouse_x, mouse_y, SDLC, FNT, FC);
    }
  }

  if (SDLC->viewing_settings && FNT->state->col_fonts_created) {
    create_col_text_bg(mouse_x, mouse_y, SDLC, FNT);
  }

  if (playing_song) {
    if (volbar_latched) {
      move_volume_bar(mouse_x, SDLC->container, SDLC->SSPtr->audio_data, SDLC->SSPtr->vol_bar);
    }

    if (scrollbar_latched) {
      move_seekbar(mouse_x, SDLC->container, SDLC->SSPtr->audio_data, SDLC->SSPtr->seek_bar);
    }
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
handle_space_key(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  i8  files_exist = FC->file_state->files_exist;
  i8* hard_stop   = &SDLC->SSPtr->pb_state->hard_stop;

  if (files_exist && !SDLC->viewing_settings) {
    *hard_stop = TRUE;
    load_song(SDLC, FC, FNT);
  }

  if (!files_exist) {
    printf("Cannot play music: No files were found.\n");
  }
}

int
status_check(i8 status[]) {
  if (status[0] && status[1]) {
    return TRUE;
  }

  return FALSE;
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
change_volume(f32* vol, f32 amount) {
  *vol = clamp(*vol, amount, 0.0f, 1.0f);
}

f32
clamp(f32 vol, f32 amount, f32 min, f32 max) {
  f32 sum = vol += amount;
  if (sum < min) {
    return min;
  }
  if (sum > max) {
    return max;
  }
  return sum;
}

void
handle_mouse_click(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  clicked_in_rect(SDLC, FNT, FC, mouse_x, mouse_y);
}

char*
check_sign(int num) {
  if (num > 0) {
    return "Positive";
  }

  if (num < 0) {
    return "Negative";
  }

  return "Zero";
}

void
handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC, FileContext* FC) {
  char* sign = check_sign(wheel_y);

  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  const int mouse_arr[] = { mouse_x, mouse_y };

  scroll_in_rect(mouse_arr, SDLC, FC, sign);
}

void
handle_mouse_release(SDLContext* SDLC, FileState* FS) {
  PlaybackState*    PBSte     = SDLC->SSPtr->pb_state;
  SeekBar*          SKBar     = SDLC->SSPtr->seek_bar;
  VolBar*           VBar      = SDLC->SSPtr->vol_bar;
  FourierTransform* FTPtr     = SDLC->FTPtr;
  i8*               is_paused = &PBSte->is_paused;

  if (PBSte->playing_song && SKBar->latched) {
    SKBar->latched = FALSE;
    play_song(FS, is_paused, &SDLC->audio_dev);
    instantiate_buffers(FTPtr->fft_buffers);
  }

  if (PBSte->playing_song && VBar->latched) {
    VBar->latched = FALSE;
  }
}

int
within_bounds_x(int x, int start, int end) {
  if (x >= start && x <= end) {
    return 1;
  }
  return 0;
}
/*
 ^^^^^^
 Bounds checks
 ^^^^^^
*/
int
point_in_rect(int x, int y, SDL_Rect rect) {
  return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
} /*point_in_rect*/

int
find_clicked_song(FontData sf_arr[], int file_count, const int mouse_arr[]) {
  /*If the pointer is within the rectangle of any of these song titles, return the id(The id is determined by
   * the index inside the loop where the fonts are created, so it's essentially an index as well)*/
  for (int i = 0; i < file_count; i++) {
    SDL_Rect sf_rect = sf_arr[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], sf_rect)) {
      return sf_arr[i].id;
    }
  }
  return -1;
} /*find_clicked_song*/

FontData*
find_clicked_theme(FontData col[], const int mouse_arr[]) {
  for (int i = 0; i < COLOUR_LIST_SIZE; i++) {
    SDL_Rect rect = col[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], rect)) {
      return &col[i];
    }
  }
  return NULL;
}

char*
find_clicked_dir(FontData df_arr[], int dir_count, const int mouse_arr[]) {
  /*
   * Getting the selection text via rect coordinates. Unrelated to the displayed text.
   */
  for (int i = 0; i < dir_count; i++) {
    SDL_Rect df_rect = df_arr[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], df_rect)) {
      return df_arr[i].text;
    }
  }
  return "NO_SELECTION";
} /*find_clicked_dir*/

void
reset_songlist_pos(Positions* pos) {
  pos->song_list_pos = 0;
} /*reset_songlist_pos*/

void
clicked_in_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x, const int mouse_y) {

  SDLSprites*   Spr        = SDLC->sprites;
  SDLViewports* Vps        = SDLC->container->vps;
  SongState*    SSPtr      = SDLC->SSPtr;
  SDL_Rect      seek_rect  = SSPtr->seek_bar->seek_box;
  SDL_Rect      vol_rect   = SSPtr->vol_bar->seek_box;
  SDL_Rect      gear_rect  = Spr->sett_gear->rect;
  SDL_Rect      pause_rect = Spr->pause_icon->rect;
  SDL_Rect      play_rect  = Spr->play_icon->rect;
  SDL_Rect      stop_rect  = Spr->stop_icon->rect;

  SDL_Rect playing_rect_array[] = { Vps->dir_vp, Vps->song_vp, gear_rect };
  SDL_Rect paused_rect_array[]  = { seek_rect, vol_rect, gear_rect, pause_rect, play_rect, stop_rect };

  i8 playing_song = SDLC->SSPtr->pb_state->playing_song;

  /*Determining the outcome based off pointer location and application state*/
  switch (playing_song) {

  default: {
    break;
  }

  case TRUE: {
    size_t len = sizeof(paused_rect_array) / sizeof(paused_rect_array[0]);
    for (size_t i = 0; i < len; i++) {
      if (point_in_rect(mouse_x, mouse_y, paused_rect_array[i])) {
        switch (i) {
        case 0: {
          grab_seek_bar(SDLC, FC->file_state, mouse_x, mouse_y);
          break;
        }

        case 1: {
          grab_vol_bar(SDLC, mouse_x, mouse_y);
          break;
        }

        case 2: {
          clicked_settings_gear(SDLC);
          break;
        }

        case 3: {
          pause_song(FC->file_state, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
          break;
        }

        case 4: {
          play_song(FC->file_state, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
          break;
        }

        case 5: {
          stop_playback(FC->file_state, SSPtr->pb_state, &SDLC->audio_dev);
          break;
        }

        default: {
          break;
        }
        }
      }
    }
    break;
  }

  case FALSE: {
    switch (SDLC->viewing_settings) {
    case TRUE: {

      if (point_in_rect(mouse_x, mouse_y, gear_rect)) {
        clicked_settings_gear(SDLC);
        return;
      }

      SDLContainer* Cont = SDLC->container;
      SDL_Rect      vp   = { 0, 0, Cont->win_width, Cont->win_height };

      if (point_in_rect(mouse_x, mouse_y, vp)) {
        const int mouse_arr[] = { mouse_x, mouse_y };
        FontData* col_ptr     = FNT->colours_list;
        FontData* col_rtn     = find_clicked_theme(col_ptr, mouse_arr);

        if (col_rtn == NULL) {
          return;
        }

        int err = update_colours(SDLC, FNT, FC, col_rtn->text);
        if (err < 0) {
          fprintf(stderr, "COULD NOT CHANGE COLOUR! : %s\n", strerror(errno));
        }
      }
      break;
    }

    case FALSE: {
      size_t len = sizeof(playing_rect_array) / sizeof(playing_rect_array[0]);
      for (size_t i = 0; i < len; i++) {
        if (point_in_rect(mouse_x, mouse_y, playing_rect_array[i])) {
          switch (i) {
          case 2: {
            clicked_settings_gear(SDLC);
            break;
          }

          case 1: {
            clicked_in_song_rect(SDLC, FNT, FC, mouse_x, mouse_y);
            break;
          }

          case 0: {
            clicked_in_dir_rect(SDLC, FNT, FC, mouse_x, mouse_y);
            break;
          }

          default: {
            break;
          }
          }
        }
      }
      break;
    }

    default: {
      break;
    }
    }
    break;
  }
  }
} /*clicked_in_rect*/

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

/*Setting the latch bools*/
void
seek_latch_on(SeekBar* SKBar, int switch_value) {
  SKBar->latched = switch_value;
}

void
vol_latch_on(VolBar* VBar, int switch_value) {
  VBar->latched = switch_value;
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
clicked_in_song_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x,
                     const int mouse_y) {
  i8   song_fonts_created = FNT->state->song_fonts_created;
  int* file_index         = &FC->file_state->file_index;

  if (song_fonts_created) {
    int offset_x = SDLC->mouse->mouse_offset_x;
    int offset_y = SDLC->mouse->mouse_offset_y;

    /*Finding the index associated with the title clicked*/
    const int mouse_arr[]     = { (mouse_x - offset_x), (mouse_y - offset_y) };
    int       file_count      = FC->file_state->file_count;
    FontData* sf_arr          = FNT->sf_arr;
    int       selection_index = find_clicked_song(sf_arr, file_count, mouse_arr);

    if (selection_index < 0) {
      return;
    }

    /*Assigning the index, and starting the song*/
    *file_index = selection_index;
    start_song_from_menu(SDLC, FC, FNT);
  }
} /*clicked_in_song_rect*/

void
clicked_in_dir_rect(SDLContext* SDLC, FontContext* FNT, FileContext* FC, const int mouse_x,
                    const int mouse_y) {
  i8 song_fonts_created = FNT->state->song_fonts_created;
  i8 dir_fonts_created  = FNT->state->dir_fonts_created;

  Positions* Pos = FNT->pos;

  // This rect is basically the pivot and doesnt require any offsets for cordinates

  if (dir_fonts_created) {
    const int mouse_arr[] = { (mouse_x), (mouse_y) };
    int       dir_count   = FC->dir_state->dir_count;
    FontData* df_arr      = FNT->df_arr;
    /*Searching for a the directory title clicked(if any)*/
    char* selection = find_clicked_dir(df_arr, dir_count, mouse_arr);

    /*Just early return if none are found*/
    if (strcmp(selection, "NO_SELECTION") == 0) {
      return;
    }

    /*Clear any existing fonts for the song title section (if any) and reset the position variable*/

    ListLimiter* LLmtr = SDLC->container->list_limiter;

    LLmtr->song_first_index = 0;
    LLmtr->song_last_index  = 0;

    clear_existing_list(df_arr, song_fonts_created, FC->file_state, selection);
    reset_songlist_pos(Pos);

    /*Getting the regular file names inside the selected directory*/
    FC->file_state->files_exist = FALSE;

    int res = fetch_files(FC->file_state);
    if (res < 0) {
      fprintf(stderr, "Error getting files : %s\n", strerror(errno));
    } else if (res == 0) {
      fprintf(stdout, "No files found.\n");
    }
    FC->file_state->file_count = res;

    /*Creating the fonts if everything went well*/
    if (res > 0) {
      FC->file_state->files_exist = TRUE;
      create_song_fonts(FNT, FC->file_state, SDLC->r);
    }
  }
} /*clicked_in_dir_rect*/

void
scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, FileContext* FC, char* sign) {
  SDLViewports* Vps   = SDLC->container->vps;
  ListLimiter*  LLmtr = SDLC->container->list_limiter;

  if (point_in_rect(mouse_arr[0], mouse_arr[1], Vps->song_vp)) {
    if (strcmp(sign, "Negative") == 0) {
      int array_increment = LLmtr->song_first_index + LLmtr->amount_to_display;
      if (array_increment >= FC->file_state->file_count) {
        array_increment = 0;
      }
      LLmtr->song_first_index = array_increment;
    }

    if (strcmp(sign, "Positive") == 0) {
      int array_increment = LLmtr->song_first_index - LLmtr->amount_to_display;
      if (array_increment < 0) {
        array_increment = 0;
      }
      LLmtr->song_first_index = array_increment;
    }
  }

  if (point_in_rect(mouse_arr[0], mouse_arr[1], Vps->dir_vp)) {
    if (strcmp(sign, "Negative") == 0) {
      int array_increment = LLmtr->dir_first_index + LLmtr->amount_to_display;
      if (array_increment >= FC->dir_state->dir_count) {
        array_increment = 0;
      }
      LLmtr->dir_first_index = array_increment;
    }

    if (strcmp(sign, "Positive") == 0) {
      int array_increment = LLmtr->dir_first_index - LLmtr->amount_to_display;
      if (array_increment < 0) {
        array_increment = 0;
      }
      LLmtr->dir_first_index = array_increment;
    }
  }
} /*scroll_in_rect*/

void
clicked_settings_gear(SDLContext* SDLC) {
  SDLC->viewing_settings = !SDLC->viewing_settings;
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

  i8 song_fonts_created = Fnt->state->song_fonts_created;
  i8 dir_fonts_created  = Fnt->state->dir_fonts_created;
  i8 col_fonts_created  = Fnt->state->col_fonts_created;

  int file_count = FC->file_state->file_count;
  int dir_count  = FC->dir_state->dir_count;

  Fnt->context_data->color = current_theme->text;

  if (song_fonts_created) {
    destroy_song_fonts(Fnt, file_count);
    create_song_fonts(Fnt, FC->file_state, SDLC->r);
  }

  if (dir_fonts_created) {
    destroy_dir_fonts(Fnt, dir_count);
    create_dir_fonts(Fnt, FC->dir_state, SDLC->r);
  }

  if (col_fonts_created) {
    destroy_colours_fonts(Fnt);
    create_colours_fonts(Fnt, themes_ptr, SDLC->r);
  }

  SDLContainer* Cont = SDLC->container;
  if (Cont->win_icon != NULL) {
    convert_pixel_colours(&Cont->win_icon, last_prim, current_theme->primary);
    SDL_SetWindowIcon(SDLC->w, Cont->win_icon);
  }

  return 0;
}
