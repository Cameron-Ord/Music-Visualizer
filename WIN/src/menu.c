#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/init.h"
#include "../inc/music_visualizer.h"

int
within_bounds_x(int x, int width) {
  if (x > (width - width) && x < width) {
    return 1;
  }
  return 0;
}

int
point_in_rect(int x, int y, SDL_Rect rect) {
  return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
} /*point_in_rect*/

int
find_clicked_song(FontData* sf_arr[], int file_count, const int mouse_arr[]) {
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
clicked_in_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y) {
  SDL_Rect dir_rect     = SDLC->container->dir_viewport;
  SDL_Rect song_rect    = SDLC->container->song_viewport;
  i8       playing_song = SDLC->SSPtr->pb_state->playing_song;

  int device_status = SDL_GetAudioDeviceStatus(SDLC->audio_dev);

  switch (playing_song) {
  default: {
    break;
  }

  case TRUE: {
    if (device_status) {
      clicked_while_active(SDLC, mouse_x, mouse_y);
    }
    break;
  }

  case FALSE: {
    if (point_in_rect(mouse_x, mouse_y, dir_rect)) {
      clicked_in_dir_rect(SDLC, mouse_x, mouse_y);
    } else if (point_in_rect(mouse_x, mouse_y, song_rect)) {
      clicked_in_song_rect(SDLC, mouse_x, mouse_y);
    }
    break;
  }
  }
} /*clicked_in_rect*/

void
clicked_while_active(SDLContext* SDLC, const int mouse_x, const int mouse_y) {
  SeekBar* SKBar = SDLC->SSPtr->seek_bar;
  VolBar*  VBar  = SDLC->SSPtr->vol_bar;

  if (point_in_rect(mouse_x, mouse_y, SKBar->vp)) {
    grab_seek_bar(SDLC, mouse_x, mouse_y);
  } else if (point_in_rect(mouse_x, mouse_y, VBar->vp)) {
    grab_vol_bar(SDLC, mouse_x, mouse_y);
  }
}

void
grab_seek_bar(SDLContext* SDLC, const int mouse_x, const int mouse_y) {

  SongState* SSPtr = SDLC->SSPtr;
  SeekBar*   SKBar = SSPtr->seek_bar;

  int win_width = SDLC->container->win_width;

  int half        = (int)(win_width * 0.25);
  int offset_diff = win_width - half;

  SDL_Rect hitbox = { SKBar->seek_box.x, SKBar->seek_box.y, SKBar->seek_box.w, SKBar->seek_box.h };

  if (point_in_rect(mouse_x - (offset_diff * 0.25), mouse_y, hitbox)) {
    seek_latch_on(SKBar, TRUE);
    if (SKBar->latched) {
      pause_song(SDLC->FCPtr->file_state, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
    }
  }
}

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

  SDL_Rect tmp = { offset_mouse_x, SKBar->seek_box.y, SKBar->seek_box.w, SKBar->seek_box.h };

  if (within_bounds_x(tmp.x, SKBar->vp.w)) {
    SKBar->seek_box.x = offset_mouse_x - SCROLLBAR_OFFSET;
    update_audio_position(ADta, SKBar);
  }
}

void
move_volume_bar(const int mouse_x, SDLContainer* SDLCntr, AudioData* ADta, VolBar* VBar) {
  int half           = (int)(SDLCntr->win_width * 0.25);
  int offset_diff    = SDLCntr->win_width - half;
  int offset_mouse_x = (mouse_x - (offset_diff * 0.75));

  SDL_Rect tmp = { offset_mouse_x, VBar->seek_box.y, VBar->seek_box.w, VBar->seek_box.h };

  if (within_bounds_x(tmp.x, VBar->vp.w)) {
    VBar->seek_box.x = offset_mouse_x - SCROLLBAR_OFFSET;
    update_vol_pos(ADta, VBar);
  }
}

void
start_song_from_menu(SDLContext* SDLC) {
  FileContext* FCPtr = SDLC->FCPtr;
  SongState*   SSPtr = SDLC->SSPtr;

  i8 files_exist  = FCPtr->file_state->files_exist;
  i8 playing_song = SSPtr->pb_state->playing_song;

  if (!playing_song && files_exist) {
    load_song(SDLC);
  } else if (playing_song && files_exist) {
    stop_playback(SDLC);
  } else {
    printf("Cannot play music: No files were found.\n");
  }
} /*start_song_from_menu*/

void
clicked_in_song_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y) {
  i8         song_fonts_created = SDLC->FntPtr->state->song_fonts_created;
  FileState* FSPtr              = SDLC->FCPtr->file_state;
  int*       file_index         = &FSPtr->file_index;

  if (song_fonts_created) {
    int offset_y = SDLC->mouse->mouse_offset_y;

    const int  mouse_arr[]     = { (mouse_x), (mouse_y - offset_y) };
    int        file_count      = FSPtr->file_count;
    FontData** sf_arr          = &SDLC->FntPtr->sf_arr;
    int        selection_index = find_clicked_song(sf_arr, file_count, mouse_arr);

    if (selection_index < 0) {
      return;
    }

    *file_index = selection_index;
    start_song_from_menu(SDLC);
  }
} /*clicked_in_song_rect*/

void
clicked_in_dir_rect(SDLContext* SDLC, const int mouse_x, const int mouse_y) {
  i8 song_fonts_created = SDLC->FntPtr->state->song_fonts_created;
  i8 dir_fonts_created  = SDLC->FntPtr->state->dir_fonts_created;

  DirState*  DSPtr = SDLC->FCPtr->dir_state;
  FileState* FSPtr = SDLC->FCPtr->file_state;
  Positions* Pos   = SDLC->FntPtr->pos;

  int win_height = SDLC->container->win_height;
  int one_tenth  = (int)(win_height * 0.05);

  if (dir_fonts_created) {
    const int  mouse_arr[] = { mouse_x, mouse_y - one_tenth };
    int        dir_count   = DSPtr->dir_count;
    FontData** df_arr      = &SDLC->FntPtr->df_arr;
    char*      selection   = find_clicked_dir(df_arr, dir_count, mouse_arr);

    if (strcmp(selection, "NO_SELECTION") == 0) {
      return;
    }

    clear_existing_list(df_arr, song_fonts_created, FSPtr, selection);
    reset_songlist_pos(Pos);

    FSPtr->files_exist = FALSE;

    int res = fetch_files(FSPtr);
    if (res < 0) {
      fprintf(stderr, "Error getting files : %s\n", strerror(errno));
      return;
    } else if (res == 0) {
      fprintf(stdout, "No files found.\n");
    }
    FSPtr->file_count  = res;
    FSPtr->files_exist = TRUE;

    create_song_fonts(SDLC->FntPtr, FSPtr, SDLC->r);
  }
} /*clicked_in_dir_rect*/

FontData*
get_struct(FontData* arr[], const int mouse_arr[], int len) {
  for (int i = 0; i < len; i++) {
    SDL_Rect rect = (*arr)[i].font_rect;
    if (point_in_rect(mouse_arr[0], mouse_arr[1], rect)) {
      return &(*arr)[i];
    } else {
      (*arr)[i].has_bg = 0;
    }
  }
  return NULL;
} /*get_*/

void
scroll_in_rect(const int mouse_arr[], SDLContext* SDLC, Sint32 wheel_y) {

  Positions* Pos = SDLC->FntPtr->pos;

  int* sl_pos  = &Pos->song_list_pos;
  int* dir_pos = &Pos->dir_list_pos;

  SDL_Rect dir_rect  = SDLC->container->dir_viewport;
  SDL_Rect song_rect = SDLC->container->song_viewport;

  int song_height = Pos->song_list_height;
  int dir_height  = Pos->dir_list_height;

  /*POSITIVE IS NEGATIVE, NEGATIVE IS POSITIVE*/

  if (point_in_rect(mouse_arr[0], mouse_arr[1], song_rect)) {
    int max_scroll = song_height - song_rect.h;
    if (max_scroll <= 0) {
      max_scroll = 0;
    }

    int scroll_amount = CONST_YPOS * wheel_y;
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

    int scroll_amount = CONST_YPOS * wheel_y;
    *dir_pos += scroll_amount;

    if (*dir_pos > 0) {
      *dir_pos = 0;
    } else if (*dir_pos < -max_scroll) {
      *dir_pos = -max_scroll;
    }
  }
} /*scroll_in_rect*/
