#include "macro.h"
#include "music_visualizer.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// int main(int argc, char* argv[]) {
// pid_t pid;
// int   result;
// pid = fork();
//
// if (pid < 0) {
// PRINT_STR_ERR(stderr, "Failed to fork process", strerror(errno));
// exit(EXIT_FAILURE);
//} else if (pid == 0) {
//
// umask(0);
// if (setsid() < 0) {
// PRINT_STR_ERR(stderr, "Failed to create session ID", strerror(errno));
// exit(EXIT_FAILURE);
//}
//
// char* home = getenv("HOME");
// if (home == NULL) {
// PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
//}
//
// if (chdir(home) < 0) {
// PRINT_STR_ERR(stderr, "Failed to chdir", strerror(errno));
// exit(EXIT_FAILURE);
//}
//
// result = music_player(argc, argv);
// exit(result);
//} else if (pid > 0) {
// exit(EXIT_SUCCESS);
//}
//
// return result;
//}

int main(int argc, char** argv) {
  setup_dirs();
  SDLContext SDLChunk;

  SDLContainer SDLContainer;
  SDLMouse     SDLMouse;

  SDLChunk.container = &SDLContainer;
  SDLChunk.mouse     = &SDLMouse;

  SDLChunk.running = TRUE;

  fprintf(stdout, "Initializing SDL..\n");

  int err;
  err = initialize_SDL();
  if (err < 0) {
    return 1;
  }
  srand(time(NULL));

  fprintf(stdout, "Creating window..\n");
  err = create_window(&SDLChunk.w);
  if (err < 0) {
    return 1;
  }

  fprintf(stdout, "Creating renderer..\n");
  err = create_renderer(&SDLChunk.w, &SDLChunk.r);
  if (err < 0) {
    SDL_DestroyWindow(SDLChunk.w);
    return 1;
  }

  FontContext FontChunk;

  TTFData    ContextData;
  FontState  FntSte;
  Positions  FntPos;
  ActiveSong Actve;

  Actve.tex  = NULL;
  Actve.surf = NULL;
  Actve.text = NULL;

  baseline_context_data(&ContextData);
  baseline_font_state(&FntSte);
  baseline_pos(&FntPos);

  fprintf(stdout, "Initialzing TTF..\n");

  err = initialize_TTF(&ContextData);
  if (err < 0) {
    return 1;
  }

  err = open_font(&ContextData);
  if (err < 0) {
    return 1;
  }

  FontChunk.df_arr = NULL;
  FontChunk.sf_arr = NULL;

  SDL_Color font_color = {189, 147, 249, 0};
  ContextData.color    = font_color;
  FntSte.initialized   = TRUE;

  FontChunk.context_data = &ContextData;
  FontChunk.state        = &FntSte;
  FontChunk.pos          = &FntPos;
  FontChunk.active       = &Actve;

  FileContext FileChunk;

  FileState FState;
  DirState  DState;

  baseline_dir_state(&DState);
  baseline_file_state(&FState);

  DState.dirs_exist = FALSE;

  fprintf(stdout, "Fetching dirs..\n");
  int res = fetch_dirs(&DState);
  if (res < 0) {
    fprintf(stderr, "Error getting directoies : %s\n", strerror(errno));
    return 1;
  } else if (res == 0) {
    fprintf(stdout, "No dirs found\n");
  }
  DState.dir_count = res;

  fprintf(stdout, "Creating fonts for dirs..\n");

  err = create_dir_fonts(&FontChunk, &DState, SDLChunk.r);
  if (err < 0) {
    return 1;
  }

  FileChunk.dir_state  = &DState;
  FileChunk.file_state = &FState;

  FourierTransform FTransform;

  FTransformData    FTransData;
  FTransformBuffers FTransBufs;

  baseline_fft_values(&FTransData);
  instantiate_buffers(&FTransBufs);

  FTransform.fft_data    = &FTransData;
  FTransform.fft_buffers = &FTransBufs;

  SongState AudioChunk;

  AudioData     ADta;
  SeekBar       SKBar;
  PlaybackState PBSte;

  baseline_audio_data(&ADta);
  baseline_seek_bar(&SKBar);
  baseline_pb_state(&PBSte);

  AudioChunk.pb_state   = &PBSte;
  AudioChunk.seek_bar   = &SKBar;
  AudioChunk.audio_data = &ADta;

  SDLChunk.FntPtr = &FontChunk;
  SDLChunk.SSPtr  = &AudioChunk;
  SDLChunk.FTPtr  = &FTransform;
  SDLChunk.FCPtr  = &FileChunk;

  fprintf(stdout, "Updating viewports..\n");
  update_viewports(&SDLChunk);

  float prev_time = SDL_GetTicks64();
  float current_time, delta_time;

  while (SDLChunk.running == TRUE) {
    current_time = SDL_GetTicks64();
    delta_time   = current_time - prev_time;

    handle_state(&SDLChunk);
    if (delta_time >= TICKS_PER_FRAME) {
      SDL_Delay(TICKS_PER_FRAME);
      prev_time = SDL_GetTicks64();
    }

    poll_events(&SDLChunk);
  }

  if (SDLChunk.w)
    SDL_DestroyRenderer(SDLChunk.r);

  if (SDLChunk.w)
    SDL_DestroyWindow(SDLChunk.w);

  if (PBSte.playing_song)
    stop_song(&AudioChunk);

  if (SDLChunk.audio_dev)
    SDL_CloseAudioDevice(SDLChunk.audio_dev);

  clear_fonts(&FontChunk, &FileChunk);
  clear_files(&FontChunk, &FileChunk);
  clear_dirs(&FontChunk, &FileChunk);

  TTF_Quit();
  SDL_Quit();

  return 0;
}

int initialize_SDL() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int create_window(SDL_Window** w) {
  *w = SDL_CreateWindow("Music Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, BWIDTH,
                        BHEIGHT, 0);
  if (!*w) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  SDL_SetWindowResizable(*w, SDL_TRUE);
  return 0;
}

int create_renderer(SDL_Window** w, SDL_Renderer** r) {
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

SDL_AudioDeviceID create_audio_device(SDL_AudioSpec* spec) {
  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0);
  if (!dev) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return dev;
}

int initialize_TTF(TTFData* cntxtdata) {
  if (TTF_Init() < 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

int open_font(TTFData* cntxtdata) {
  cntxtdata->font = TTF_OpenFont(FONT_PATH, LRG);
  if (cntxtdata->font == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return -1;
  }
  return 0;
}

void get_window_container_size(SDL_Window* w, SDLContainer* SDLCnt) {
  SDL_GetWindowSize(w, &SDLCnt->win_width, &SDLCnt->win_height);
}

void zero_buffers(FTransformData* FTData, FTransformBuffers* FTBuf) {
  int DS = FTData->DS_AMOUNT;
  memset(FTBuf->in_left, 0, N * sizeof(float));
  memset(FTBuf->in_right, 0, N * sizeof(float));
  memset(FTBuf->combined_window, 0, N * sizeof(float));
  memset(FTBuf->out_raw, 0, N * sizeof(float _Complex));
  memset(FTBuf->processed, 0, HALF_BUFF * sizeof(float));
  memset(FTBuf->smoothed, 0, HALF_BUFF * sizeof(float));
  memset(FTBuf->out_log, 0, N * sizeof(float));
}

void reset_playback_variables(AudioData* Aud, PlaybackState* PBste) {
  Aud->audio_pos  = 0;
  PBste->is_ready = FALSE;
  Aud->wav_len    = 0;
  Aud->buffer     = free_ptr(Aud->buffer);
}

void* free_ptr(void* ptr) {
  if (ptr != NULL) {
    free(ptr);
  }
  return NULL;
}

int read_to_buffer(SDLContext* SDLC) {
#ifdef __LINUX__

  char* home = getenv("HOME");
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Err getting home DIR", strerror(errno));
    return -1;
  }

  SongState*   SSPtr = SDLC->SSPtr;
  FileContext* FCPtr = SDLC->FCPtr;

  FileState* FS  = FCPtr->file_state;
  AudioData* Aud = SSPtr->audio_data;

  char combined_path[PATH_MAX];
  char path[PATH_MAX];

  snprintf(path, PATH_MAX, "%s/Music/fftmplayer/%s/", home, FS->selected_dir);
  sprintf(combined_path, "%s%s", path, FS->files[FS->file_index]);
  printf("\nREADING FILE : %s\n\n", FS->files[FS->file_index]);

  SNDFILE* sndfile;
  SF_INFO  sfinfo;

  sndfile = sf_open(combined_path, SFM_READ, &sfinfo);
  if (!sndfile) {
    PRINT_STR_ERR(stderr, "Err opening file for reading", strerror(errno));
    return -1;
  }

  if (sfinfo.channels != 2) {
    fprintf(stderr, "Must be a 2 channel audio file!\n");
    return -1;
  }

  Aud->channels = sfinfo.channels;
  Aud->sr       = sfinfo.samplerate;
  Aud->format   = sfinfo.format;

  printf("--CHANNELS : %d\n", Aud->channels);
  printf("--SAMPLE RATE : %d\n", Aud->sr);
  printf("--FORMAT : %x\n", Aud->format);
  printf("--FRAMES :  %ld\n", sfinfo.frames);

  Aud->samples = sfinfo.frames * sfinfo.channels;
  Aud->buffer  = malloc(Aud->samples * sizeof(float));
  if (Aud->buffer == NULL) {
    return -1;
  }

  memset(Aud->buffer, 0, Aud->samples * sizeof(float));
  printf("--BUFFER : %p\n", Aud->buffer);

  sf_count_t num_read = sf_read_float(sndfile, Aud->buffer, Aud->samples);
  if (num_read < 0) {
    PRINT_STR_ERR(stderr, "Err reading audio data", strerror(errno));
    free_ptr(Aud->buffer);
    sf_close(sndfile);
    return -1;
  }

  Aud->wav_len = Aud->samples;

  FourierTransform* FTPtr = SDLC->FTPtr;

  float DSf                  = (float)N / FTPtr->fft_data->DS_AMOUNT;
  FTPtr->fft_data->freq_step = (float)Aud->sr / DSf;
  printf("\n..Done reading. Closing file\n\n");

  sf_close(sndfile);

  return 0;
#endif

  return -1;
}

/*I would like to implement a volume control but i also want to not loop in my callback for
 * performance reasons but whatever, could use paralell idk*/
void callback(void* data, Uint8* stream, int len) {
  SDLContext*       SDLCPtr = (struct SDLContext*)data;
  SongState*        SSPtr   = SDLCPtr->SSPtr;
  FourierTransform* FTPtr   = SDLCPtr->FTPtr;

  AudioData* Aud = SSPtr->audio_data;

  int remaining_samples = (Aud->wav_len - Aud->audio_pos);

  int samples_to_copy =
      (len / sizeof(float) < remaining_samples) ? len / sizeof(float) : remaining_samples;

  float* f32_stream = (float*)stream;

  // for (int i = 0; i < samples_to_copy; i++) {
  // f32_stream[i] = Aud->buffer[i + Aud->audio_pos];
  //}
  memcpy(f32_stream, Aud->buffer + Aud->audio_pos, samples_to_copy * sizeof(float));
  if (Aud->audio_pos > 0 && Aud->audio_pos < Aud->wav_len) {
    fft_push(FTPtr, SSPtr, SDLCPtr->spec.channels, samples_to_copy * sizeof(float));
  }

  Aud->audio_pos += samples_to_copy;

  if (Aud->audio_pos >= Aud->wav_len) {
    fprintf(stdout, "\n AUDIO POS: %d, WAV_LEN: %d\n", Aud->audio_pos, Aud->wav_len);
    fprintf(stdout, "End reached.. Starting next song.\n");
    SSPtr->pb_state->song_ended = TRUE;
  }
}

void print_spec_data(SDL_AudioSpec spec, SDL_AudioDeviceID dev) {
  printf(
      "\nFORMAT : %d\n CHANNELS: %d\n FREQ: %d\n USERDATA: %p\n CALLBACK %p\n SAMPLES: %d\n SIZE "
      ": %d\n",
      spec.format, spec.channels, spec.freq, spec.userdata, spec.callback, spec.samples, spec.size);
  printf("\nDEVICE ID : %d\n", dev);
}

void update_audio_position(AudioData* ADta, SeekBar* SKBar) {
  int ttl_length       = SKBar->vp.w;
  int current_position = SKBar->seek_box.x + SCROLLBAR_OFFSET;
  if (current_position < 0.0 || current_position > SKBar->vp.w) {
    return;
  }
  f32 normalized  = ((float)current_position / (float)ttl_length);
  int scaled_pos  = normalized * ADta->wav_len;
  ADta->audio_pos = scaled_pos;
}

void poll_events(SDLContext* SDLC) {

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {

    default: {
      break;
    }

    case SDL_QUIT: {
      SDLC->running = FALSE;
      break;
    }

    case SDL_MOUSEBUTTONDOWN: {
      handle_mouse_click(SDLC);
      break;
    }

    case SDL_MOUSEBUTTONUP: {
      handle_mouse_release(SDLC);
      break;
    }

    case SDL_MOUSEMOTION: {
      handle_mouse_motion(SDLC);
      break;
    }

    case SDL_MOUSEWHEEL: {
      if (!SDLC->SSPtr->pb_state->playing_song)
        handle_mouse_wheel(e.wheel.y, SDLC);
      break;
    }

    case SDL_KEYDOWN: {

      switch (e.key.keysym.sym) {

      default: {
        break;
      }

      case SDLK_r: {
        random_song(SDLC);
        break;
      }

      case SDLK_q: {
        SDLC->running = FALSE;
        break;
      }

      case SDLK_p: {
        toggle_pause(SDLC);
        break;
      }

      case SDLK_SPACE: {
        handle_space_key(SDLC);
        break;
      }

      case SDLK_RIGHT: {
        next_song(SDLC);
        break;
      }

      case SDLK_LEFT: {
        prev_song(SDLC);
        break;
      }

      }
    }

    case SDL_WINDOWEVENT: {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        update_viewports(SDLC);
        break;
      }
      default: {
        break;
      }
      }
      break;
    }
    }
  }
}
