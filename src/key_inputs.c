#include "../inc/audio.h"
#include "../inc/music_visualizer.h"

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
