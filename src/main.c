#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/init.h"
#include "../inc/music_visualizer.h"
#include <SDL2/SDL_main.h>
#include <errno.h>

int
main(int argc, char* argv[]) {

/*Optional daemonize with args. Not gonna bother implementing the win32 version using create process*/
#ifdef __linux__
  int result = 0;
  if (argc >= 2) {
    if (strcmp(argv[1], "--daemonize=yes") == 0) {
      pid_t pid;
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
        /*Entry point*/
        result = music_player();
        exit(result);
      } else if (pid > 0) {
        exit(EXIT_SUCCESS);
      }

      return result;
    }
  }
#endif

  if (argc < 2) {
    /*Entry point*/
    int result = music_player();
    return result;
  }

  return 0;
}

int
music_player() {
  /*Creating the folders for the application if they don't exist, and rerouting stdout and stderr to files for
   * logging*/
  // setup_dirs();

  AppContext Application;

  SDLContext   SDLChunk;
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

  err = initialize_TTF();
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
  } else if (res == 0) {
    fprintf(stdout, "No dirs found\n");
  }
  DState.dir_count = res;

  if (res > 0) {
    fprintf(stdout, "Creating fonts for dirs..\n");
    DState.dirs_exist = TRUE;

    err = create_dir_fonts(&FontChunk, &DState, SDLChunk.r);
    if (err < 0) {
      return 1;
    }
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
  VolBar        VLBar;
  PlaybackState PBSte;

  baseline_audio_data(&ADta);
  baseline_seek_bar(&SKBar);
  baseline_vol_bar(&VLBar);
  baseline_pb_state(&PBSte);

  AudioChunk.pb_state   = &PBSte;
  AudioChunk.seek_bar   = &SKBar;
  AudioChunk.audio_data = &ADta;
  AudioChunk.vol_bar    = &VLBar;

  SDLChunk.FTPtr = &FTransform;
  SDLChunk.SSPtr = &AudioChunk;

  FTransform.SSPtr = &AudioChunk;
  FTransform.SDLC  = &SDLChunk;

  AudioChunk.FTPtr = &FTransform;
  AudioChunk.SDLC  = &SDLChunk;

  Application.SDLC   = &SDLChunk;
  Application.FTPtr  = &FTransform;
  Application.FCPtr  = &FileChunk;
  Application.FntPtr = &FontChunk;
  Application.SSPtr  = &AudioChunk;

  /*Calling update viewports here to instantiate the values and ensure that things are placed relatively*/

  update_viewports(SDLChunk.container, SDLChunk.mouse, SDLChunk.w);
  resize_fonts(&SDLChunk, &FileChunk, &FontChunk);

  float prev_time = SDL_GetTicks64();
  float current_time, delta_time;

  while (SDLChunk.running == TRUE) {
    current_time = SDL_GetTicks64();
    delta_time   = current_time - prev_time;

    handle_state(&Application);
    if (delta_time >= TICKS_PER_FRAME) {
      SDL_Delay(TICKS_PER_FRAME);
      prev_time = SDL_GetTicks64();
    }

    poll_events(&Application);
  }

  /*Clean up*/

  if (SDLChunk.w) {
    SDL_DestroyRenderer(SDLChunk.r);
  }

  if (SDLChunk.w) {
    SDL_DestroyWindow(SDLChunk.w);
  }

  if (PBSte.playing_song) {
    stop_playback(&FState, &PBSte, &SDLChunk.audio_dev);
  }

  if (SDLChunk.audio_dev) {
    SDL_CloseAudioDevice(SDLChunk.audio_dev);
  }

  clear_fonts(&FontChunk, &FileChunk);
  clear_files(&FileChunk);
  clear_dirs(&FileChunk);

  ADta.buffer = free_ptr(ADta.buffer);

  TTF_Quit();
  SDL_Quit();

  return 0;
}

void
poll_events(AppContext* app) {

  SDLContext* SDLC = app->SDLC;
  AudioData*  AD   = app->SSPtr->audio_data;

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
      handle_mouse_click(SDLC, app->FntPtr, app->FCPtr);
      break;
    }

    case SDL_MOUSEBUTTONUP: {
      handle_mouse_release(SDLC, app->FCPtr->file_state);
      break;
    }

    case SDL_MOUSEMOTION: {
      handle_mouse_motion(SDLC, app->FntPtr, app->FCPtr);
      break;
    }

    case SDL_MOUSEWHEEL: {
      if (!SDLC->SSPtr->pb_state->playing_song) handle_mouse_wheel(e.wheel.y, SDLC, app->FntPtr);
      break;
    }

    case SDL_KEYDOWN: {

      switch (e.key.keysym.sym) {

      default: {
        break;
      }

      case SDLK_r: {
        random_song(SDLC, app->FCPtr, app->FntPtr);
        break;
      }

      case SDLK_q: {
        SDLC->running = FALSE;
        break;
      }

      case SDLK_p: {
        toggle_pause(SDLC, app->FCPtr->file_state);
        break;
      }

      case SDLK_SPACE: {
        handle_space_key(SDLC, app->FntPtr, app->FCPtr);
        break;
      }

      case SDLK_RIGHT: {
        next_song(SDLC, app->FntPtr, app->FCPtr);
        break;
      }

      case SDLK_LEFT: {
        prev_song(SDLC, app->FCPtr, app->FntPtr);
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
      break;
    }

    case SDL_WINDOWEVENT: {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        update_viewports(SDLC->container, SDLC->mouse, SDLC->w);
        resize_fonts(SDLC, app->FCPtr, app->FntPtr);
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
