#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

void
free_all(AppContext* a) {

  fprintf(stdout, "Cleaning up..\n");

  if (a->SDLC->r) {
    SDL_DestroyRenderer(a->SDLC->r);
  }

  if (a->SDLC->w) {
    SDL_DestroyWindow(a->SDLC->w);
  }

  if (a->SSPtr->pb_state->playing_song) {
    a->SSPtr->pb_state->hard_stop = TRUE;
    stop_playback(NULL, a->SSPtr->pb_state, &a->SDLC->audio_dev);
  }

  SDLSprites* s = a->SDLC->sprites;

  if (s->play_icon->surf) {
    SDL_FreeSurface(s->play_icon->surf);
  }

  if (s->play_icon->tex) {
    SDL_DestroyTexture(s->play_icon->tex);
  }

  if (s->pause_icon->surf) {
    SDL_FreeSurface(s->pause_icon->surf);
  }

  if (s->pause_icon->tex) {
    SDL_DestroyTexture(s->pause_icon->tex);
  }

  if (s->stop_icon->surf) {
    SDL_FreeSurface(s->stop_icon->surf);
  }

  if (s->stop_icon->tex) {
    SDL_DestroyTexture(s->stop_icon->tex);
  }

  if (s->sett_gear->surf) {
    SDL_FreeSurface(s->sett_gear->surf);
  }

  if (s->sett_gear->tex) {
    SDL_DestroyTexture(s->sett_gear->tex);
  }

  if (s->seek_icon->surf) {
    SDL_FreeSurface(s->seek_icon->surf);
  }

  if (s->seek_icon->tex) {
    SDL_DestroyTexture(s->seek_icon->tex);
  }

  if (a->SDLC->container->win_icon) {
    SDL_FreeSurface(a->SDLC->container->win_icon);
  }

  destroy_dir_fonts(a->FntPtr, a->FCPtr->dir_state->dir_count);
  destroy_song_fonts(a->FntPtr, a->FCPtr->file_state->file_count);
  destroy_colours_fonts(a->FntPtr);
  clear_dirs(a->FCPtr);
  clear_files(a->FCPtr);

  free_ptr(a->SSPtr->audio_data->buffer);
  fprintf(stdout, "Finished cleaning up\n");
}

void*
destroy_surface(SDL_Surface* surf) {
  if (surf != NULL) {
    SDL_FreeSurface(surf);
  }
  return NULL;
}

void*
destroy_texture(SDL_Texture* tex) {
  if (tex != NULL) {
    SDL_DestroyTexture(tex);
  }
  return NULL;
}

void
clear_dirs(FileContext* FCPtr) {
  i8*  dirs_exist = &FCPtr->dir_state->dirs_exist;
  int* dir_count  = &FCPtr->dir_state->dir_count;

  if (*dir_count > 0 && *dirs_exist) {
    for (int i = 0; i < *dir_count; i++) {
      free_ptr(FCPtr->dir_state->directories[i]);
    }
  }

  if (*dirs_exist) {
    free_ptr(FCPtr->dir_state->directories);
  }

  *dir_count  = 0;
  *dirs_exist = FALSE;
}

void
clear_files(FileContext* FCPtr) {
  i8*  files_exist = &FCPtr->file_state->files_exist;
  int* file_count  = &FCPtr->file_state->file_count;

  if (*file_count > 0 && *files_exist) {
    for (int i = 0; i < *file_count; i++) {
      free_ptr(FCPtr->file_state->files[i]);
    }
  }

  if (*files_exist) {
    free_ptr(FCPtr->file_state->files);
  }

  *file_count  = 0;
  *files_exist = FALSE;
}

void
clear_existing_list(FontData sf_arr[], int song_fonts_created, FileState* FSPtr, char* selection) {
  FSPtr->selected_dir = selection;
  if (FSPtr->files_exist && FSPtr->file_count > 0) {
    for (int i = 0; i < FSPtr->file_count; i++) {
      free_ptr(FSPtr->files[i]);
    }

    FSPtr->file_count = 0;
  }

  if (FSPtr->files_exist) {
    free_ptr(FSPtr->files);
  }

  if (song_fonts_created) {
    for (int i = 0; i < FSPtr->file_count; i++) {
      sf_arr[i].font_texture = destroy_texture(sf_arr[i].font_texture);
    }
  }
} /*clear_existing_list*/

void
destroy_dir_fonts(FontContext* Fnt, int dir_count) {
  for (int i = 0; i < dir_count; i++) {
    Fnt->df_arr[i].font_texture = destroy_texture(Fnt->df_arr[i].font_texture);
  }
  free_ptr(Fnt->df_arr);
  Fnt->state->dir_fonts_created = FALSE;
}

void
destroy_song_fonts(FontContext* Fnt, int file_count) {
  for (int i = 0; i < file_count; i++) {
    Fnt->sf_arr[i].font_texture = destroy_texture(Fnt->sf_arr[i].font_texture);
  }
  free_ptr(Fnt->sf_arr);
  Fnt->state->song_fonts_created = FALSE;
}

int
destroy_colours_fonts(FontContext* Fnt) {
  for (int s = 0; s < COLOUR_LIST_SIZE; s++) {
    SDL_Texture* tex = Fnt->colours_list[s].font_texture;
    tex              = destroy_texture(tex);
  }

  Fnt->state->col_fonts_created = FALSE;
  return 0;
}
