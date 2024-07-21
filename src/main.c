#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <errno.h>

int
main(int argc, char* argv[]) {
  /*Creating the folders for the application if they don't exist, and rerouting stdout and stderr to files for
   * logging*/

  // setup_dirs();

  AppContext   Application = { 0 };
  SDLContext   SDLChunk    = { 0 };
  SDLColours   SDLTheme    = { 0 };
  SDLSprites   SDLSpr      = { 0 };
  SDLViewports SDLVps      = { 0 };
  SDLContainer SDLCont     = { 0 };
  ListLimiter  LLmtr       = { 0 };
  SDLMouse     SDLCursor   = { 0 };

  // default to 4
  LLmtr.amount_to_display = 4;

  // Default to dracula tri colour theme
  SDL_Color prim_drac = { 187, 147, 249, 255 };
  SDL_Color sec_drac  = { 40, 42, 54, 255 };
  SDL_Color tert_drac = { 69, 71, 90, 255 };

  SDL_Color prim_nord = { 236, 239, 244, 255 };
  SDL_Color sec_nord  = { 49, 52, 64, 255 };
  SDL_Color tert_nord = { 136, 192, 208, 255 };

  Theme nord = { .prim = prim_nord, .secondary = sec_nord, .tertiary = tert_nord, .name = "Nord" };
  Theme drac = { .prim = prim_drac, .secondary = sec_drac, .tertiary = tert_drac, .name = "Dracula" };

  Theme* themes[] = { &nord, &drac };

  SDLTheme.primary   = prim_nord;
  SDLTheme.secondary = sec_nord;
  SDLTheme.tertiary  = tert_nord;
  SDLTheme.themes    = themes;

  SDLCont.theme        = &SDLTheme;
  SDLCont.vps          = &SDLVps;
  SDLCont.list_limiter = &LLmtr;

  SDLChunk.sprites   = &SDLSpr;
  SDLChunk.container = &SDLCont;
  SDLChunk.mouse     = &SDLCursor;
  SDLChunk.running   = TRUE;

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

  FontContext FontChunk = { 0 };
  FontState   FntSte    = { 0 };
  Positions   FntPos    = { 0 };
  ActiveSong  Actve     = { 0 };

  TTFData ContextData = { .font_size = 64, .font = NULL, .color = SDLTheme.primary };

  err = initialize_TTF();
  if (err < 0) {
    return 1;
  }

  err = open_font(&ContextData);
  if (err < 0) {
    return 1;
  }

  FntSte.initialized = TRUE;

  FontChunk.context_data = &ContextData;
  FontChunk.state        = &FntSte;
  FontChunk.pos          = &FntPos;
  FontChunk.active       = &Actve;

  FileContext FileChunk = { 0 };

  FileState FState = { 0 };
  DirState  DState = { 0 };

  DState.dirs_exist = FALSE;

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

  err = create_colours_fonts(&FontChunk, themes, SDLChunk.r);
  if (err < 0) {
    return 1;
  }

  FileChunk.dir_state  = &DState;
  FileChunk.file_state = &FState;

  FourierTransform  FTransform = { 0 };
  FTransformData    FTransData = { 0 };
  FTransformBuffers FTransBufs = { 0 };

  FTransData.max_ampl = 1.0f;
  FTransBufs.in_ptr   = NULL;

  instantiate_buffers(&FTransBufs);

  FTransform.fft_data    = &FTransData;
  FTransform.fft_buffers = &FTransBufs;

  SongState AudioChunk = { 0 };

  AudioData     ADta  = { 0 };
  SeekBar       SKBar = { 0 };
  VolBar        VLBar = { 0 };
  PlaybackState PBSte = { 0 };

  ADta.volume = 1.0f;

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

  err = initialize_sdl_image();
  if (err < 0) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  SDL_Color from_colour = { 255, 255, 255, 255 };

  SettingsGear Gear = { 0 };

  Gear.surf = load_image(SETTINGS_ICON_PATH);
  if (Gear.surf == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  convert_pixel_colours(&Gear.surf, from_colour, SDLTheme.primary);
  set_rect(&Gear.rect, 0, 0, SIZE32, SIZE32);
  Gear.tex = create_image_texture(SDLChunk.r, Gear.surf);
  if (Gear.tex == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  PlayIcon Play = { 0 };

  Play.surf = load_image(PLAY_ICON_PATH);
  if (Play.surf == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  convert_pixel_colours(&Play.surf, from_colour, SDLTheme.tertiary);
  set_rect(&Play.rect, 0, 0, SIZE16, SIZE16);

  Play.tex = create_image_texture(SDLChunk.r, Play.surf);
  if (Play.tex == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  PauseIcon Pause = { 0 };

  Pause.surf = load_image(PAUSE_ICON_PATH);
  if (Pause.surf == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  convert_pixel_colours(&Pause.surf, from_colour, SDLTheme.tertiary);
  set_rect(&Pause.rect, 0, 0, SIZE16, SIZE16);

  Pause.tex = create_image_texture(SDLChunk.r, Pause.surf);
  if (Pause.tex == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  StopIcon Stop = { 0 };

  Stop.surf = load_image(STOP_ICON_PATH);
  if (Stop.surf == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  convert_pixel_colours(&Stop.surf, from_colour, SDLTheme.tertiary);
  set_rect(&Stop.rect, 0, 0, SIZE16, SIZE16);

  Stop.tex = create_image_texture(SDLChunk.r, Stop.surf);
  if (Stop.tex == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  SeekIcon Seek = { 0 };

  Seek.surf = load_image(SEEKER_ICON_PATH);
  if (Seek.surf == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  convert_pixel_colours(&Seek.surf, from_colour, SDLTheme.tertiary);

  const size_t main_seekers = 2;

  for (size_t i = 0; i < main_seekers; i++) {
    set_rect(&Seek.rect_main[i], 0, 0, SIZE16, SIZE16);
  }

  Seek.tex = create_image_texture(SDLChunk.r, Seek.surf);
  if (Seek.tex == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
    return 1;
  }

  SDLChunk.sprites->sett_gear  = &Gear;
  SDLChunk.sprites->play_icon  = &Play;
  SDLChunk.sprites->pause_icon = &Pause;
  SDLChunk.sprites->stop_icon  = &Stop;
  SDLChunk.sprites->seek_icon  = &Seek;

  Application.SDLC   = &SDLChunk;
  Application.FTPtr  = &FTransform;
  Application.FCPtr  = &FileChunk;
  Application.FntPtr = &FontChunk;
  Application.SSPtr  = &AudioChunk;

  /*Calling update viewports here to instantiate the values and ensure that things are placed relatively*/
  update_window_size(SDLChunk.container, SDLChunk.w);
  resize_fonts(&SDLChunk, &FileChunk, &FontChunk);

  SDL_EnableScreenSaver();

  u32 frame_start;
  int frame_time;

  while (SDLChunk.running == TRUE) {
    frame_start = SDL_GetTicks64();

    handle_state(&Application);
    poll_events(&Application);

    frame_time = SDL_GetTicks64() - frame_start;

    if (TICKS_PER_FRAME > frame_time) {
      SDL_Delay(TICKS_PER_FRAME - frame_time);
    }
  }

  /*Clean up*/

  if (SDLChunk.r) {
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

  if (Stop.tex) {
    SDL_DestroyTexture(Stop.tex);
  }

  if (Pause.tex) {
    SDL_DestroyTexture(Pause.tex);
  }

  if (Play.tex) {
    SDL_DestroyTexture(Play.tex);
  }

  if (Gear.tex) {
    SDL_DestroyTexture(Gear.tex);
  }

  if (Seek.tex) {
    SDL_DestroyTexture(Seek.tex);
  }

  clear_fonts(&FontChunk, &FileChunk);
  clear_files(&FileChunk);
  clear_dirs(&FileChunk);

  free_ptr(ADta.buffer);

  IMG_Quit();
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
      if (!SDLC->SSPtr->pb_state->playing_song) handle_mouse_wheel(e.wheel.y, SDLC, app->FCPtr);
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

      case SDL_WINDOWEVENT_RESIZED: {
        update_window_size(SDLC->container, SDLC->w);
        resize_fonts(SDLC, app->FCPtr, app->FntPtr);
        break;
      }

      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        update_window_size(SDLC->container, SDLC->w);
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

/*Kinda hard to get this right with tiling WMs so im just gonna leave it out for now*/

void
check_size(SDLContainer* Cont, SDL_Window* w) {
  if (Cont->win_width < BWIDTH && Cont->win_height < BHEIGHT) {
    SDL_SetWindowSize(w, BWIDTH, BHEIGHT);
  }

  if (Cont->win_width < BWIDTH && Cont->win_height > BHEIGHT) {
    SDL_SetWindowSize(w, BWIDTH, Cont->win_height);
  }

  if (Cont->win_width > BWIDTH && Cont->win_height < BHEIGHT) {
    SDL_SetWindowSize(w, Cont->win_width, BHEIGHT);
  }
}
