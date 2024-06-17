#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"

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
find_clicked_song(FontData* sf_arr[], int file_count, const int mouse_arr[]) {
  /*If the pointer is within the rectangle of any of these song titles, return the id(The id is determined by
   * the index inside the loop where the fonts are created, so it's essentially an index as well)*/
  for (int i = 0; i < file_count; i++) {
    SDL_Rect sf_rect = (*sf_arr)[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], sf_rect)) {
      return (*sf_arr)[i].id;
    }
  }
  return -1;
} /*find_clicked_song*/

char*
find_clicked_dir(FontData* df_arr[], int dir_count, const int mouse_arr[]) {
  /*
   * Getting the selection text via rect coordinates. Unrelated to the displayed text.
   */
  for (int i = 0; i < dir_count; i++) {
    if (point_in_rect(mouse_arr[0], mouse_arr[1], (*df_arr)[i].font_rect)) {
      return (*df_arr)[i].text;
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

  SDLSprites*   Spr       = SDLC->sprites;
  SDLViewports* Vps       = SDLC->container->vps;
  SDL_Rect      gear_rect = Spr->sett_gear->rect;

  SDL_Rect playing_rect_array[] = { Vps->dir_vp, Vps->song_vp, gear_rect };
  SDL_Rect paused_rect_array[]  = { Vps->controls_vp, Vps->controls_vp, gear_rect };

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

        default: {
          break;
        }
        }
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
  SDLViewports* Vps = SDLCntr->vps;

  SDL_Rect controls_vp = Vps->controls_vp;

  int line_x     = controls_vp.w * 0.2;
  int sub_amount = (controls_vp.w * 0.20) / 2;

  if (within_bounds_x(mouse_x, (line_x - sub_amount), (line_x - sub_amount) + controls_vp.w * 0.20)) {
    SKBar->seek_box.x = mouse_x - SCROLLBAR_OFFSET;
    update_audio_position(ADta, SKBar, Vps->controls_vp);
  }
}

void
move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar) {
  SDLViewports* Vps = SDLCntr->vps;

  SDL_Rect controls_vp = Vps->controls_vp;

  int line_x     = controls_vp.w * 0.80;
  int sub_amount = (controls_vp.w * 0.20) / 2;

  if (within_bounds_x(mouse_x, (line_x - sub_amount), (line_x - sub_amount) + controls_vp.w * 0.20)) {
    /*Implementing a slight offset here so that it looks a bit nicer, this is compensated for.*/
    VBar->seek_box.x = mouse_x - SCROLLBAR_OFFSET;

    /*Set the volume off where the mouse is*/
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
  i8            song_fonts_created = FNT->state->song_fonts_created;
  int*          file_index         = &FC->file_state->file_index;
  SDLContainer* SDLCntrPtr         = SDLC->container;

  if (song_fonts_created) {
    int offset_x = SDLC->mouse->mouse_offset_x;
    int offset_y = SDLC->mouse->mouse_offset_y;

    /*Finding the index associated with the title clicked*/
    const int  mouse_arr[]     = { (mouse_x - offset_x), (mouse_y - offset_y) };
    int        file_count      = FC->file_state->file_count;
    FontData** sf_arr          = &FNT->sf_arr;
    int        selection_index = find_clicked_song(sf_arr, file_count, mouse_arr);

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
    int offset_y = SDLC->mouse->mouse_offset_y;

    const int  mouse_arr[] = { (mouse_x), (mouse_y) };
    int        dir_count   = FC->dir_state->dir_count;
    FontData** df_arr      = &FNT->df_arr;
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

FontData*
get_struct(FontData* arr[], const int mouse_arr[], int len) {
  /*If the pointer is within the rectangle of any of these titles, return a pointer to the pointer of the
   * FontData struct array, otherwise clear the background)*/
  for (int i = 0; i < len; i++) {
    SDL_Rect rect = (*arr)[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], rect)) {
      return &(*arr)[i];
    } else {
      (*arr)[i].has_bg = 0;
    }
  }
  return NULL;
} /*get_struct*/

void
scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, FileContext* FC, char* sign) {
  SDLViewports* Vps   = SDLC->container->vps;
  ListLimiter*  LLmtr = SDLC->container->list_limiter;

  if (point_in_rect(mouse_arr[0], mouse_arr[1], Vps->song_vp)) {
    if (strcmp(sign, "Negative") == 0) {
      int array_increment = LLmtr->song_first_index + LLmtr->amount_to_display;
      if (array_increment + 1 > FC->file_state->file_count) {
        array_increment = 0;
      }
      LLmtr->song_first_index = array_increment;
    }

    if (strcmp(sign, "Positive") == 0) {
      int array_increment = LLmtr->song_first_index - LLmtr->amount_to_display;
      if (array_increment + 1 < 0) {
        array_increment = 0;
      }
      LLmtr->song_first_index = array_increment;
    }
  }

  if (point_in_rect(mouse_arr[0], mouse_arr[1], Vps->dir_vp)) {
    if (strcmp(sign, "Negative") == 0) {
      int array_increment = LLmtr->dir_first_index + LLmtr->amount_to_display;
      if (array_increment + 1 > FC->dir_state->dir_count) {
        array_increment = 0;
      }
      LLmtr->dir_first_index = array_increment;
    }

    if (strcmp(sign, "Positive") == 0) {
      int array_increment = LLmtr->dir_first_index - LLmtr->amount_to_display;
      if (array_increment + 1 < 0) {
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
