#include "macro.h"
#include "music_visualizer.h"

void baseline_context_data(TTFData* cntxt) {
  memset(&cntxt->color, 0, sizeof(SDL_Color));
  cntxt->font      = NULL;
  cntxt->font_path = FONT_PATH;
}

void baseline_font_state(FontState* state) {
  state->dir_fonts_created  = FALSE;
  state->song_fonts_created = FALSE;
  state->initialized        = FALSE;
}

void baseline_pos(Positions* pos) {
  pos->dir_list_pos    = 0;
  pos->dir_list_height = 0;
  pos->dir_list_offset = 0;

  pos->song_list_pos    = 0;
  pos->song_list_height = 0;
  pos->song_list_offset = 0;
}

void baseline_dir_state(DirState* dir) {
  dir->dir_count   = 0;
  dir->dir_index   = 0;
  dir->dirs_exist  = FALSE;
  dir->directories = NULL;
}

void baseline_file_state(FileState* file) {
  file->file_count   = 0;
  file->file_index   = 0;
  file->files_exist  = FALSE;
  file->selected_dir = NULL;
  file->files        = NULL;
}

void baseline_audio_data(AudioData* data) { data->buffer = NULL; }

void baseline_seek_bar(SeekBar* skbar) { skbar->latched = FALSE; }

void baseline_pb_state(PlaybackState* pbste) {
  pbste->hard_stop         = FALSE;
  pbste->is_paused         = FALSE;
  pbste->playing_song      = FALSE;
  pbste->is_ready          = FALSE;
  pbste->song_ended        = FALSE;
  pbste->currently_playing = FALSE;
}

void baseline_fft_values(FTransformData* data) {
  data->DS_AMOUNT     = DEFAULT_DS;
  data->buffers_ready = FALSE;
  data->freq_step     = 0.0f;
  data->cell_width    = 0;
  data->output_len    = 0;
}

void instantiate_buffers(FTransformBuffers* bufs) {
  memset(bufs->combined_window, 0, sizeof(f32) * N);
  memset(bufs->out_log, 0, sizeof(f32) * N);
  memset(bufs->out_raw, 0, sizeof(f32c) * N);
  memset(bufs->smoothed, 0, sizeof(f32) * (N / 2));
  memset(bufs->processed, 0, sizeof(f32) * (N / 2));
}

void set_spec_data(SDLContext* SDLC) {
  SDL_AudioSpec* spec = &SDLC->spec;
  AudioData*     ad   = SDLC->SSPtr->audio_data;

  spec->userdata = SDLC;
  spec->callback = callback;
  spec->channels = ad->channels;
  spec->freq     = ad->sr;
  spec->format   = AUDIO_F32;
  spec->samples  = N;
  spec->size     = 0;
  spec->padding  = 0;
  spec->silence  = 0;
}
