#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"
#include "../inc/utils.h"

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

  SDL_Rect paused_rect_array[]  = { Vps->dir_vp, Vps->song_vp, gear_rect };
  SDL_Rect playing_rect_array[] = { seek_rect, vol_rect, pause_rect, play_rect, stop_rect };

  i8 playing_song = SDLC->SSPtr->pb_state->playing_song;

  /*Determining the outcome based off pointer location and application state*/
  switch (playing_song) {

  default: {
    break;
  }

  case TRUE: {
    size_t len = sizeof(playing_rect_array) / sizeof(playing_rect_array[0]);
    for (size_t i = 0; i < len; i++) {
      if (point_in_rect(mouse_x, mouse_y, playing_rect_array[i])) {
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
          pause_song(FC->file_state, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
          break;
        }

        case 3: {
          play_song(FC->file_state, &SSPtr->pb_state->is_paused, &SDLC->audio_dev);
          break;
        }

        case 4: {
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
      size_t len = sizeof(paused_rect_array) / sizeof(paused_rect_array[0]);
      for (size_t i = 0; i < len; i++) {
        if (point_in_rect(mouse_x, mouse_y, paused_rect_array[i])) {
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

    int err;
    err = verify_directory_existence(selection);
    if (err != 1) {
      destroy_dir_fonts(FNT, dir_count);
      destroy_song_fonts(FNT, FC->file_state->file_count);
      clear_dirs(FC);
      clear_files(FC);

      int res = fetch_dirs(FC->dir_state);
      if (res < 0) {
        PRINT_STR_ERR(stderr, "Error getting directories : %s", strerror(errno));
        return;
      } else if (res == 0) {
        fprintf(stdout, "No directories found\n");
        return;
      }

      FC->dir_state->dir_count = res;
      if (res > 0) {
        FC->dir_state->dirs_exist = TRUE;

        err = create_dir_fonts(FNT, FC->dir_state, SDLC->r);
        if (err < 0) {
          PRINT_STR_ERR(stderr, "FAILED TO CREATE NEW FONTS FOR DIRECTORIES! ERRNO : %s", strerror(errno));
          return;
        }
      }
    }

    /*Clear any existing fonts for the song title section (if any) and reset the position variable*/

    ListLimiter* LLmtr = SDLC->container->list_limiter;

    LLmtr->song_first_index = 0;
    LLmtr->song_last_index  = 0;

    clear_existing_list(df_arr, song_fonts_created, FC->file_state, selection);

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

void
handle_mouse_wheel(Sint32 wheel_y, SDLContext* SDLC, FileContext* FC) {
  char* sign = check_sign(wheel_y);

  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  const int mouse_arr[] = { mouse_x, mouse_y };

  scroll_in_rect(mouse_arr, SDLC, FC, sign);
}

void
handle_mouse_click(SDLContext* SDLC, FontContext* FNT, FileContext* FC) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  clicked_in_rect(SDLC, FNT, FC, mouse_x, mouse_y);
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
