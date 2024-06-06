#include "../inc/init.h"
#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/music_visualizer.h"

int
initialize_SDL() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int
create_window(SDL_Window** w) {
  *w = SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BWIDTH, BHEIGHT,
                        0);
  if (!*w) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  SDL_SetWindowResizable(*w, SDL_TRUE);
  return 0;
}

int
create_renderer(SDL_Window** w, SDL_Renderer** r) {
  if (*w) {
    *r = SDL_CreateRenderer(*w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*r) {
      PRINT_SDL_ERR(stderr, SDL_GetError());
      SDL_DestroyWindow(*w);
      return -1;
    }
    return 0;
  }
  return -1;
}

SDL_AudioDeviceID
create_audio_device(SDL_AudioSpec* spec) {
  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0);
  if (!dev) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return dev;
}

int
initialize_TTF() {
  if (TTF_Init() < 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int
open_font(TTFData* cntxtdata) {
  cntxtdata->font = TTF_OpenFont(FONT_PATH, cntxtdata->font_size);
  if (cntxtdata->font == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

void
get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt) {
  SDL_GetWindowSize(w, &SDLCnt->win_width, &SDLCnt->win_height);
}

void
baseline_context_data(TTFData* cntxt) {
  memset(&cntxt->color, 0, sizeof(SDL_Color));
  cntxt->font      = NULL;
  cntxt->font_path = FONT_PATH;
  cntxt->font_size = 16;
}

void
baseline_font_state(FontState* state) {
  state->dir_fonts_created  = FALSE;
  state->song_fonts_created = FALSE;
  state->initialized        = FALSE;
}

void
baseline_pos(Positions* pos) {
  pos->dir_list_pos    = 0;
  pos->dir_list_height = 0;
  pos->dir_list_offset = 0;

  pos->song_list_pos    = 0;
  pos->song_list_height = 0;
  pos->song_list_offset = 0;
}

void
baseline_dir_state(DirState* dir) {
  dir->dir_count   = 0;
  dir->dir_index   = 0;
  dir->dirs_exist  = FALSE;
  dir->directories = NULL;
}

void
baseline_file_state(FileState* file) {
  file->file_count   = 0;
  file->file_index   = 0;
  file->files_exist  = FALSE;
  file->selected_dir = NULL;
  file->files        = NULL;
}

void
baseline_audio_data(AudioData* data) {
  data->buffer = NULL;
  data->volume = 1.0f;
}

void
baseline_seek_bar(SeekBar* skbar) {
  skbar->latched = FALSE;
}

void
baseline_vol_bar(VolBar* vlbar) {
  vlbar->latched = FALSE;
}

void
baseline_pb_state(PlaybackState* pbste) {
  pbste->hard_stop         = FALSE;
  pbste->is_paused         = FALSE;
  pbste->playing_song      = FALSE;
  pbste->is_ready          = FALSE;
  pbste->song_ended        = FALSE;
  pbste->currently_playing = FALSE;
}

void
baseline_fft_values(FTransformData* data) {
  data->DS_AMOUNT     = DEFAULT_DS;
  data->buffers_ready = FALSE;
  data->fft_ready     = FALSE;
  data->freq_step     = 0.0f;
  data->cell_width    = 0;
  data->output_len    = 0;
  data->max_ampl      = 1.0f;
}

void
instantiate_buffers(FTransformBuffers* bufs) {
  memset(bufs->fft_in, 0, DOUBLE_BUFF * sizeof(f32));
  memset(bufs->in_cpy, 0, BUFF_SIZE * sizeof(float _Complex));
  memset(bufs->out_raw, 0, BUFF_SIZE * sizeof(float _Complex));
  memset(bufs->processed, 0, (BUFF_SIZE / 2) * sizeof(float));
  memset(bufs->smoothed, 0, (BUFF_SIZE / 2) * sizeof(float));
}

void
set_spec_data(SDLContext* SDLC) {
  SDL_AudioSpec* spec = &SDLC->spec;
  AudioData*     ad   = SDLC->SSPtr->audio_data;

  spec->userdata = SDLC;
  spec->callback = callback;
  spec->channels = ad->channels;
  spec->freq     = ad->sr;
  spec->format   = AUDIO_F32;
  spec->samples  = BUFF_SIZE;
}
