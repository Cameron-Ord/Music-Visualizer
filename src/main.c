#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/graphics.h"
#include "../inc/init.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

int
main(int argc, char* argv[]) {

  AppContext   Application = { 0 };
  SDLContext   SDLChunk    = { 0 };
  SDLColours   SDLTheme    = { 0 };
  SDLSprites   SDLSpr      = { 0 };
  SDLViewports SDLVps      = { 0 };
  SDLContainer SDLCont     = { 0 };
  ListLimiter  LLmtr       = { 0 };
  SDLMouse     SDLCursor   = { 0 };

  PathContainer p = setup_dirs();
  if (p.is_valid) {
    strcpy(SDLChunk.log_path, p.log_path_cpy);
    strcpy(SDLChunk.err_log_path, p.err_log_path_cpy);
  }

  fprintf(stdout, "LOG PATH -> %s\n", SDLChunk.log_path);
  fprintf(stderr, "ERROR LOG PATH -> %s\n", SDLChunk.err_log_path);

  FILE* stdout_file = freopen(SDLChunk.log_path, "a", stdout);
  if (stdout_file != NULL) {
  } else {
    PRINT_STR_ERR(stderr, "Error redirecting STDOUT to file", strerror(errno));
  }

  FILE* stderr_file = freopen(SDLChunk.err_log_path, "a", stderr);
  if (stderr_file != NULL) {
  } else {
    PRINT_STR_ERR(stderr, "Error redirecting STDERR to file", strerror(errno));
  }

  // default to 4
  LLmtr.amount_to_display = 4;

  // Default to dracula tri colour theme
  SDL_Color prim_drac = { 255, 121, 198, 255 };
  // bg
  SDL_Color sec_drac = { 40, 42, 54, 255 };
  // aux (bars - seekers - etc)
  SDL_Color tert_drac = { 189, 147, 249, 255 };
  // text
  SDL_Color text_drac   = { 248, 248, 242, 255 };
  SDL_Color textbg_drac = { 98, 114, 164, 255 };

  SDL_Color prim_nord = { 129, 161, 193, 255 };
  // bg
  SDL_Color sec_nord = { 46, 52, 64, 255 };
  // aux (bars - seekers - etc)
  SDL_Color tert_nord = { 136, 192, 208, 255 };
  // text
  SDL_Color text_nord   = { 216, 222, 233, 255 };
  SDL_Color textbg_nord = { 94, 129, 172, 255 };

  Theme nord = { .prim      = prim_nord,
                 .secondary = sec_nord,
                 .tertiary  = tert_nord,
                 .text      = text_nord,
                 .text_bg   = textbg_nord,
                 .name      = "Nord" };

  Theme drac = { .prim      = prim_drac,
                 .secondary = sec_drac,
                 .tertiary  = tert_drac,
                 .text      = text_drac,
                 .text_bg   = textbg_drac,
                 .name      = "Dracula" };

  Theme* themes[] = { &nord, &drac };

  SDLTheme.primary   = prim_nord;
  SDLTheme.secondary = sec_nord;
  SDLTheme.tertiary  = tert_nord;
  SDLTheme.text      = text_nord;
  SDLTheme.textbg    = textbg_nord;
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

  SDL_EnableScreenSaver();

  FontContext FontChunk = { 0 };
  FontState   FntSte    = { 0 };
  Positions   FntPos    = { 0 };
  ActiveSong  Actve     = { 0 };

  TTFData ContextData = { .font_size = 64, .font = NULL, .color = SDLTheme.text };

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
    PRINT_STR_ERR(stderr, "Error getting directories : %s", strerror(errno));
  } else if (res == 0) {
    fprintf(stdout, "No directories found\n");
  }
  DState.dir_count = res;

  if (res > 0) {
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

  SDLCont.win_icon = NULL;
  SDLCont.win_icon = load_image(MUSIC_ICON_PATH);
  if (SDLCont.win_icon == NULL) {
    PRINT_SDL_ERR(stderr, SDL_GetError());
  }

  if (SDLCont.win_icon) {
    convert_pixel_colours(&SDLCont.win_icon, from_colour, SDLTheme.primary);
    SDL_SetWindowIcon(SDLChunk.w, SDLCont.win_icon);
  }

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

  convert_pixel_colours(&Play.surf, from_colour, SDLTheme.primary);
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

  convert_pixel_colours(&Pause.surf, from_colour, SDLTheme.primary);
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

  convert_pixel_colours(&Stop.surf, from_colour, SDLTheme.primary);
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

  convert_pixel_colours(&Seek.surf, from_colour, SDLTheme.primary);

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

  fprintf(stdout, "Quiting SDL2 Image..\n");
  IMG_Quit();
  fprintf(stdout, "Quiting SDL2 TTF..\n");
  TTF_Quit();
  fprintf(stdout, "Quiting SDL2..\n");
  SDL_Quit();

  if (stdout_file) {
    fclose(stdout_file);
  }
  if (stderr_file) {
    fclose(stderr_file);
  }
  return 0;
}
