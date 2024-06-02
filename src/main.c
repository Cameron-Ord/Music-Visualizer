#include "audio.h"
#include "font.h"
#include "init.h"
#include "music_visualizer.h"
#include "threads.h"

int
main(int argc, char** argv) {

  if (argc >= 2) {
    if (strcmp(argv[1], "--daemonize=yes") == 0) {
      pid_t pid;
      int   result;
      pid = fork();

      if (pid < 0) {
        PRINT_STR_ERR(stderr, "Failed to fork process", strerror(errno));
        exit(EXIT_FAILURE);
      } else if (pid == 0) {

        umask(0);
        if (setsid() < 0) {
          PRINT_STR_ERR(stderr, "Failed to create session ID", strerror(errno));
          exit(EXIT_FAILURE);
        }

        char* home = getenv("HOME");
        if (home == NULL) {
          PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
        }

        if (chdir(home) < 0) {
          PRINT_STR_ERR(stderr, "Failed to chdir", strerror(errno));
          exit(EXIT_FAILURE);
        }

        result = music_player();
        exit(result);
      } else if (pid > 0) {
        exit(EXIT_SUCCESS);
      }

      return result;
    }
  }

  if (argc < 2) {
    int result = music_player();
    return result;
  }

  return 0;
}

int
music_player() {
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

  SDL_Color font_color = { 189, 147, 249, 0 };
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
  FTransform.winwkr = NULL;

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

  int cores = sysconf(_SC_NPROCESSORS_ONLN);
  printf("Cores : %d\n", cores);

  WindowWorker* winwkr = NULL;

  winwkr = malloc(sizeof(WindowWorker) * cores);
  if (winwkr == NULL) {
    PRINT_STR_ERR(stderr, "Thread workers could not be allocated.. Fallback to single core.",
                  strerror(errno));
    return 1;
  }

  winwkr->thread = NULL;

  err = create_window_workers(winwkr, cores);
  if (err < 0) {
    PRINT_STR_ERR(stderr, "Thread workers could not be allocated.. Fallback to single core.",
                  strerror(errno));
    return 1;
  }

  FTransform.winwkr = winwkr;

  SDLChunk.FntPtr = &FontChunk;
  SDLChunk.SSPtr  = &AudioChunk;
  SDLChunk.FTPtr  = &FTransform;
  SDLChunk.FCPtr  = &FileChunk;

  update_viewports(SDLChunk.container, SDLChunk.mouse, SDLChunk.w);
  resize_fonts(&SDLChunk);

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

  if (SDLChunk.w) SDL_DestroyRenderer(SDLChunk.r);

  if (SDLChunk.w) SDL_DestroyWindow(SDLChunk.w);

  if (PBSte.playing_song) stop_song(&AudioChunk.pb_state->playing_song);

  if (SDLChunk.audio_dev) SDL_CloseAudioDevice(SDLChunk.audio_dev);

  clear_fonts(&FontChunk, &FileChunk);
  clear_files(&FontChunk, &FileChunk);
  clear_dirs(&FontChunk, &FileChunk);
  destroy_window_workers(winwkr, cores);

  for (int i = 0; i < cores; i++) {
    winwkr[i].thread = free_ptr(winwkr[i].thread);
  }

  winwkr      = free_ptr(winwkr);
  ADta.buffer = free_ptr(ADta.buffer);

  TTF_Quit();
  SDL_Quit();

  return 0;
}

void
poll_events(SDLContext* SDLC) {

  AudioData* AD = SDLC->SSPtr->audio_data;

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
      if (!SDLC->SSPtr->pb_state->playing_song) handle_mouse_wheel(e.wheel.y, SDLC);
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

      case SDLK_UP: {
        change_volume(&AD->volume, 0.1);
        break;
      }

      case SDLK_DOWN: {
        change_volume(&AD->volume, -0.1);
        break;
      }
      }
    }

    case SDL_WINDOWEVENT: {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        update_viewports(SDLC->container, SDLC->mouse, SDLC->w);
        resize_fonts(SDLC);
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

void*
free_ptr(void* ptr) {
  if (ptr != NULL) {
    free(ptr);
  }
  return NULL;
}
