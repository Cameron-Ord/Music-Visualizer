#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"

int
within_bounds_x(int x, int width) {
  if (x > (width - width) && x < width) {
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

  SDL_Rect dir_rect  = SDLC->container->dir_viewport;
  SDL_Rect song_rect = SDLC->container->song_viewport;
  SDL_Rect gear_rect = SDLC->gear_ptr->rect;

  SDL_Rect rect_array[] = { dir_rect, song_rect, gear_rect };

  i8 playing_song = SDLC->SSPtr->pb_state->playing_song;

  int device_status = SDL_GetAudioDeviceStatus(SDLC->audio_dev);
  /*Determining the outcome based off pointer location and application state*/
  switch (playing_song) {
  default: {
    break;
  }

  case TRUE: {
    if (device_status) {
      clicked_while_active(SDLC, FC->file_state, mouse_x, mouse_y);
    }
    break;
  }

  case FALSE: {
    for (size_t i = 0; i < sizeof(rect_array) / sizeof(rect_array[0]); i++) {
      if (point_in_rect(mouse_x, mouse_y, rect_array[i])) {
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
clicked_while_active(SDLContext* SDLC, FileState* FS, const int mouse_x, const int mouse_y) {
  SeekBar*      SKBar = SDLC->SSPtr->seek_bar;
  VolBar*       VBar  = SDLC->SSPtr->vol_bar;
  SettingsGear* Gptr  = SDLC->gear_ptr;

  SDL_Rect rect_arr[] = { SKBar->vp, VBar->vp, Gptr->rect };
  for (size_t i = 0; i < sizeof(rect_arr) / sizeof(rect_arr[0]); i++) {
    if (point_in_rect(mouse_x, mouse_y, rect_arr[i])) {
      switch (i) {
      case 0: {
        grab_seek_bar(SDLC, FS, mouse_x, mouse_y);
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
}

void
grab_seek_bar(SDLContext* SDLC, FileState* FS, const int mouse_x, const int mouse_y) {

  SongState* SSPtr = SDLC->SSPtr;
  SeekBar*   SKBar = SSPtr->seek_bar;

  int win_width = SDLC->container->win_width;

  int half        = (int)(win_width * 0.25);
  int offset_diff = win_width - half;
  /*If pointer is within bounds, "latch" onto the bar so that it follows the pointer on mouse motion
   * events*/
  SDL_Rect hitbox = { SKBar->seek_box.x, SKBar->seek_box.y, SKBar->seek_box.w, SKBar->seek_box.h };
  if (point_in_rect(mouse_x - (offset_diff * 0.25), mouse_y, hitbox)) {
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

  int win_width = SDLC->container->win_width;

  int half        = (int)(win_width * 0.25);
  int offset_diff = win_width - half;
  /*If pointer is within bounds, "latch" onto the bar so that it follows the pointer on mouse motion
   * events*/
  SDL_Rect hitbox = { VBar->seek_box.x, VBar->seek_box.y, VBar->seek_box.w, VBar->seek_box.h };
  if (point_in_rect(mouse_x - (offset_diff * 0.75), mouse_y, hitbox)) {
    vol_latch_on(VBar, TRUE);
  }
}

void
move_seekbar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, SeekBar* SKBar) {
  int half           = (int)(SDLCntr->win_width * 0.25);
  int offset_diff    = SDLCntr->win_width - half;
  int offset_mouse_x = (mouse_x - (offset_diff * 0.25));

  /*Checking if the mouse pointer is inside the bounds of the viewport containing the bar*/
  SDL_Rect tmp = { offset_mouse_x, SKBar->seek_box.y, SKBar->seek_box.w, SKBar->seek_box.h };

  if (within_bounds_x(tmp.x, SKBar->vp.w)) {
    /*Implementing a slight offset here so that it looks a bit nicer, this is compensated for in terms of
     * audio positioning*/
    SKBar->seek_box.x = offset_mouse_x - SCROLLBAR_OFFSET;
    /*Set the audio pos based off where the mouse is*/
    update_audio_position(ADta, SKBar);
  }
}

void
move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar) {
  int half           = (int)(SDLCntr->win_width * 0.25);
  int offset_diff    = SDLCntr->win_width - half;
  int offset_mouse_x = (mouse_x - (offset_diff * 0.75));

  /*Checking if the mouse pointer is inside the bounds of the viewport containing the bar*/
  SDL_Rect tmp = { offset_mouse_x, VBar->seek_box.y, VBar->seek_box.w, VBar->seek_box.h };

  if (within_bounds_x(tmp.x, VBar->vp.w)) {
    /*Implementing a slight offset here so that it looks a bit nicer, this is compensated for.*/
    VBar->seek_box.x = offset_mouse_x - SCROLLBAR_OFFSET;

    /*Set the volume off where the mouse is*/
    update_vol_pos(ADta, VBar);
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

    /*This works for the time being, but maybe create a positions struct to manage this better*/
    if (SDLCntrPtr->win_width < 800) {

      int fourty_percent = (int)(SDLCntrPtr->win_height * 0.4);

      int offset_h = SDLCntrPtr->win_height - fourty_percent;

      offset_y = fourty_percent + (offset_h * 0.2);
    }

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

  if (dir_fonts_created) {
    int offset_y = SDLC->mouse->mouse_offset_y;

    const int  mouse_arr[] = { (mouse_x), (mouse_y - offset_y) };
    int        dir_count   = FC->dir_state->dir_count;
    FontData** df_arr      = &FNT->df_arr;
    /*Searching for a the directory title clicked(if any)*/
    char* selection = find_clicked_dir(df_arr, dir_count, mouse_arr);

    /*Just early return if none are found*/
    if (strcmp(selection, "NO_SELECTION") == 0) {
      return;
    }

    /*Clear any existing fonts for the song title section (if any) and reset the position variable*/

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
scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, FontContext* FNT, Sint32 wheel_y) {

  Positions* Pos = FNT->pos;

  int* sl_pos  = &Pos->song_list_pos;
  int* dir_pos = &Pos->dir_list_pos;

  SDL_Rect dir_rect  = SDLC->container->dir_viewport;
  SDL_Rect song_rect = SDLC->container->song_viewport;

  int song_height = Pos->song_list_height;
  int dir_height  = Pos->dir_list_height;

  /*POSITIVE IS NEGATIVE, NEGATIVE IS POSITIVE*/

  /*Pretty basic scroll function, I want to redo this*/

  if (point_in_rect(mouse_arr[0], mouse_arr[1], song_rect)) {
    int max_scroll = song_height - song_rect.h;
    if (max_scroll <= 0) {
      max_scroll = 0;
    }

    int scroll_amount = 75 * wheel_y;
    *sl_pos += scroll_amount;

    if (*sl_pos > 0) {
      *sl_pos = 0;
    } else if (*sl_pos < -max_scroll) {
      *sl_pos = -max_scroll;
    }
  }

  if (point_in_rect(mouse_arr[0], mouse_arr[1], dir_rect)) {
    int max_scroll = dir_height - dir_rect.h;
    if (max_scroll <= 0) {
      max_scroll = 0;
    }

    int scroll_amount = 75 * wheel_y;
    *dir_pos += scroll_amount;

    if (*dir_pos > 0) {
      *dir_pos = 0;
    } else if (*dir_pos < -max_scroll) {
      *dir_pos = -max_scroll;
    }
  }
} /*scroll_in_rect*/

void
clicked_settings_gear(SDLContext* SDLC) {
  SDLC->viewing_settings = !SDLC->viewing_settings;
}
