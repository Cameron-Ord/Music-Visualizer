#include "../include/fft.hpp"
#include "../include/files.hpp"
#include "../include/globals.hpp"
#include "../include/paths.hpp"
#include "../include/switch.hpp"
#include "../include/theme.hpp"
#include "../include/threads.hpp"

// Globals
SDL2INTERNAL sdl2;
SDL2Audio sdl2_ad;
SDL2Renderer rend;
SDL2Window win;
SDL2KeyInputs key;
SDL2Fonts fonts;

void set_config_colours(ProgramThemes *themes, FILE *file_ptr) {
  int r, g, b, a;

  SDL_Color primary = {0, 0, 0, 0};
  SDL_Color secondary = {0, 0, 0, 0};
  SDL_Color background = {0, 0, 0, 0};
  SDL_Color text = {0, 0, 0, 0};
  SDL_Color textbg = {0, 0, 0, 0};

  const char *colors_str_buffer[] = {"primary", "secondary", "background",
                                     "text", "textbg"};
  SDL_Color *sdl_color_buffer[] = {&primary, &secondary, &background, &text,
                                   &textbg};

  int accumulator = 0;
  int total = 4 * 5;

  for (size_t i = 0;
       i < sizeof(colors_str_buffer) / sizeof(colors_str_buffer[0]); i++) {
    fseek(file_ptr, 0, SEEK_SET);

    char buffer[256];
    char pattern[256];

    snprintf(pattern, sizeof(pattern), "%s = {%%d,%%d,%%d,%%d}",
             colors_str_buffer[i]);

    while (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
      if (sscanf(buffer, pattern, &r, &g, &b, &a) == 4) {
        sdl_color_buffer[i]->r = (uint8_t)r;
        sdl_color_buffer[i]->g = (uint8_t)g;
        sdl_color_buffer[i]->b = (uint8_t)b;
        sdl_color_buffer[i]->a = (uint8_t)a;

        accumulator += 4;
      }
    }
  }

  fclose(file_ptr);

  if (accumulator != total) {
    return;
  }

  themes->set_color(primary, PRIMARY);
  themes->set_color(secondary, SECONDARY);
  themes->set_color(background, BACKGROUND);
  themes->set_color(text, TEXT);
  themes->set_color(textbg, TEXT_BG);
}

int main(int argc, char **argv) {
  int no_mouse_grab = 0;

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--no-mouse-grab") {
        no_mouse_grab = 1;
      } else {
        std::cout << "Unknown argument: " << arg << std::endl;
      }
    }
  }

  key.set_mouse_grab(no_mouse_grab);

  bool err;

  FourierTransform *fft = new FourierTransform;
  AudioData *ad = new AudioData;
  AudioDataContainer *audio_data = new AudioDataContainer;
  ProgramThemes themes;
  ProgramPath pathing;
  ProgramFiles files;

  audio_data->buffer = NULL;
  audio_data->channels = 0;
  audio_data->bytes = 0;
  audio_data->format = 0;
  audio_data->length = 0;
  audio_data->position = 0;
  audio_data->samples = 0;
  audio_data->SR = 0;
  audio_data->volume = 1.0;

  fft->set_audio_data_ptr(audio_data);
  ad->set_audio_data_ptr(audio_data);
  ad->get_audio_data()->fft_push_fn = fft_push;

  err = pathing.create_music_source();
  if (!err) {
    std::cerr << "Could not create/confirm directories! -> EXIT" << std::endl;
    return 1;
  }

  err = pathing.create_log_directories();
  if (!err) {
    std::cerr << "Could not create/confirm logging directories! -> EXIT"
              << std::endl;
    return 1;
  }

  const std::string logging_src_path = pathing.get_logging_path();

  const std::string log_file_concat =
      pathing.join_str(logging_src_path, "log.txt");

  FILE *stdout_file = NULL;
  stdout_file = freopen(log_file_concat.c_str(), "a", stdout);
  if (!stdout_file) {
    std::cerr << "Could not open logging file for stdout! ->" << strerror(errno)
              << std::endl;
  }

  const std::string errlog_file_concat =
      pathing.join_str(logging_src_path, "errlog.txt");

  FILE *stderr_file = NULL;
  stderr_file = freopen(errlog_file_concat.c_str(), "a", stderr);
  if (!stderr_file) {
    std::cerr << "Could not open logging file for stdout! ->" << strerror(errno)
              << std::endl;
  }

  FILE *conf_file = NULL;

  conf_file = fopen("config.txt", "r");
  if (!conf_file) {
    std::cerr << "Error opening file or no config file! -> " << strerror(errno)
              << std::endl;
  }

  if (conf_file) {
    set_config_colours(&themes, conf_file);
  }

  if (!sdl2.initialize_sdl2_video()) {
    std::cerr << "Failed to initialize SDL2 video!" << std::endl;
    return 1;
  }

  win.create_window(win.get_window());
  if (win.get_window() == NULL) {
    std::cerr << "Could not create window!" << std::endl;
    SDL_Quit();
    return 1;
  }

  rend.create_renderer(win.get_window(), rend.get_renderer());
  if (*rend.get_renderer() == NULL) {
    std::cerr << "Could not create renderer! -> EXIT" << std::endl;
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_events()) {
    std::cerr << "Failed to initialize SDL2 inputs! -> EXIT" << std::endl;
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_audio()) {
    std::cerr << "Failed to initialize SDL2 audio! -> EXIT" << std::endl;
    SDL_Quit();
    return 1;
  }

  if (!sdl2.initialize_sdl2_ttf()) {
    std::cerr << "Failed to initialize SDL2 TTF! -> EXIT" << std::endl;
    SDL_Quit();
    return 1;
  }

  if (!fonts.open_font()) {
    std::cerr << "Failed to open font! -> EXIT" << std::endl;
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_SetRenderDrawBlendMode(*rend.get_renderer(), SDL_BLENDMODE_BLEND);
  SDL_EnableScreenSaver();

  WIN_SIZE sizes = sdl2.get_current_window_size(*win.get_window());
  sdl2.set_window_size(sizes);
  rend.set_font_draw_limit(sizes.HEIGHT);
  fonts.set_char_limit(sizes.WIDTH);

  err = files.fill_directories(pathing.get_src_path(), pathing.return_slash());
  if (!err) {
    std::cout << "No directories" << std::endl;
  }

  fonts.create_dir_text(*files.retrieve_directories(), *themes.get_text());

  fonts.create_settings_text(*themes.get_text(), fft->get_settings());

  fonts.create_float_number_text(*themes.get_text());

  fonts.create_integer_number_text(*themes.get_text());

  ThreadData FTransformThread = {
      NULL, NULL, NULL, 1, 0, false, fft, sdl2_ad.get_stream_flag(), NULL};
  FTransformThread.c = SDL_CreateCond();
  FTransformThread.m = SDL_CreateMutex();

  FTransformThread.thread_ptr =
      SDL_CreateThread(FFT_THREAD, "FFT_THREAD_WORKER", &FTransformThread);

  const int ticks_per_frame = (1000.0 / 60);
  uint64_t frame_start;
  int frame_time;

  sdl2.set_play_state(true);
  sdl2.set_current_user_state(AT_DIRECTORIES);

  while (sdl2.get_play_state()) {
    frame_start = SDL_GetTicks64();

    rend.render_bg(themes.get_background());
    rend.render_clear();

    key.input_handler(&pathing, &files, ad, fft, &themes);
    sdl2_ad.audio_state_handler(ad, &files, &pathing, &FTransformThread);

    SDL_LockMutex(FTransformThread.m);
    FTransformThread.is_ready = 1;
    SDL_CondSignal(FTransformThread.c);
    SDL_UnlockMutex(FTransformThread.m);

    SDL_LockMutex(FTransformThread.m);
    FTransformThread.is_ready = 0;
    SDL_CondSignal(FTransformThread.c);
    SDL_UnlockMutex(FTransformThread.m);

    rend.render_state_handler(&themes, fft->get_data(), fft->get_bufs());

    frame_time = SDL_GetTicks64() - frame_start;
    if (ticks_per_frame > frame_time) {
      SDL_Delay(ticks_per_frame - frame_time);
    }

    rend.render_present();
  }

  sdl2_ad.pause_audio();
  sdl2_ad.close_audio_device();
  int th_status;

  lock_mutex(FTransformThread.m, &FTransformThread.is_locked);
  FTransformThread.is_running = 0;
  SDL_CondSignal(FTransformThread.c);
  unlock_mutex(FTransformThread.m, &FTransformThread.is_locked);

  SDL_WaitThread(FTransformThread.thread_ptr, &th_status);

  SDL_DestroyCond(FTransformThread.c);
  SDL_DestroyMutex(FTransformThread.m);

  std::cout << "FFT Thread exited with status -> " << th_status << std::endl;

  fonts.destroy_allocated_fonts();

  free(ad->get_audio_data()->buffer);
  delete ad->get_audio_data();
  delete ad;
  delete fft;

  if (*rend.get_renderer()) {
    SDL_DestroyRenderer(*rend.get_renderer());
  }

  if (*win.get_window()) {
    SDL_DestroyWindow(*win.get_window());
  }

  if (fonts.get_font_ptr()) {
    TTF_CloseFont(fonts.get_font_ptr());
  }

  if (stdout_file) {
    fclose(stdout_file);
  }

  if (stderr_file) {
    fclose(stderr_file);
  }

  TTF_Quit();
  SDL_Quit();
  return 0;
}
