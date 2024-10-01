#include "../include/audio.hpp"
#include "../include/events.hpp"
#include "../include/fft.hpp"
#include "../include/files.hpp"
#include "../include/font_entity.hpp"
#include "../include/switch.hpp"
#include "../include/program_path.hpp"
#include "../include/sdl2_entity.hpp"
#include "../include/window_entity.hpp"
#include "../include/render_entity.hpp"
#include "../include/theme.hpp"
#include "../include/mouse.hpp"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_stdinc.h"

#include <cstdio>
#include <csignal>
#include <SDL2/SDL.h>
#include <fstream>
#include <mutex>
#include "../include/threads.hpp"


std::mutex log_mutex;

void log_bad_term(const std::string *message);
void signal_handler(int signum);
void set_config_colours(ProgramThemes *themes, FILE *file_ptr);

void log_bad_term(const std::string *message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log_file("exit_log.txt", std::ios::app);
    if (log_file.is_open()) {
        log_file << message << std::endl;
    }
}

void signal_handler(int signum) {
    if (signum == SIGSEGV) {
        const std::string message = "SIGSEGV OCCURRED";
        log_bad_term(&message);
    }
}

void set_config_colours(ProgramThemes *themes, FILE *file_ptr) {
    
    int r, g, b, a;

    SDL_Color primary = {0,0,0,0};
    SDL_Color secondary = {0,0,0,0};
    SDL_Color background = {0,0,0,0};
    SDL_Color text = {0,0,0,0};
    SDL_Color textbg = {0,0,0,0};


    const char *colors_str_buffer[] = {"primary", "secondary", "background", "text", "textbg"};
    SDL_Color *sdl_color_buffer[] = {&primary, &secondary, &background, &text, &textbg};
    
    int accumulator = 0;
    int total = 4 * 5;

    for(size_t i = 0; i < sizeof(colors_str_buffer) /  sizeof(colors_str_buffer[0]); i++){
        fseek(file_ptr, 0, SEEK_SET);

        char buffer[256];
        char pattern[256];

        snprintf(pattern, sizeof(pattern), "%s = {%%d,%%d,%%d,%%d}", colors_str_buffer[i]);

        while(fgets(buffer, sizeof(buffer), file_ptr) != NULL){
            if(sscanf(buffer, pattern, &r, &g, &b, &a) == 4){
                sdl_color_buffer[i]->r =  (uint8_t) r;
                sdl_color_buffer[i]->g =  (uint8_t) g;
                sdl_color_buffer[i]->b =  (uint8_t) b;
                sdl_color_buffer[i]->a =  (uint8_t) a;

                accumulator += 4; 
            } 
        }
    }

    if(accumulator != total){
        return;
    }


    fclose(file_ptr);

    themes->set_color(primary, PRIMARY);
    themes->set_color(secondary, SECONDARY);
    themes->set_color(background, BACKGROUND);
    themes->set_color(text, TEXT);
    themes->set_color(textbg, TEXT_BG);
}

int main(int argc, char **argv) {

    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    bool err;

    USERDATA *userdata = new USERDATA;
    FourierTransform *fft = new FourierTransform;
    SDL2INTERNAL *sdl2 = new SDL2INTERNAL;
    SDL2Audio sdl2_ad;
    AudioData *ad = new AudioData;
    SDL2Renderer rend;
    SDL2Window win;
    SDL2KeyInputs key;
    ProgramThemes themes;
    ProgramPath pathing;
    ProgramFiles files;
    SDL2Fonts fonts;

    StdClassWrapper std = { ad, &pathing, &files, fft, &themes };

    SDL2Wrapper sdl2_w = { sdl2, &sdl2_ad, &rend, &win, &key, &fonts };

    userdata->ad = ad;
    userdata->sdl2_ad = &sdl2_ad;
    userdata->fft = fft;

    err = pathing.create_music_source();
    if (!err) {
        std::cerr << "Could not create/confirm directories! -> EXIT"
                  << std::endl;
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
        std::cerr << "Could not open logging file for stdout! ->"
                  << strerror(errno) << std::endl;
    }

    const std::string errlog_file_concat =
        pathing.join_str(logging_src_path, "errlog.txt");

    FILE *stderr_file = NULL;
    stderr_file = freopen(errlog_file_concat.c_str(), "a", stderr);
    if (!stderr_file) {
        std::cerr << "Could not open logging file for stdout! ->"
                  << strerror(errno) << std::endl;
    }

    FILE *conf_file = NULL;

    conf_file = fopen("config.txt", "r");
    if (!conf_file) {
        std::cerr << "Error opening file or no config file! -> "
                  << strerror(errno) << std::endl;
    }

    if (conf_file) {
        set_config_colours(&themes, conf_file);
    }

    if (!sdl2->initialize_sdl2_video()) {
        std::cerr << "Failed to initialize SDL2 video!" << std::endl;
        return 1;
    }

    if (!sdl2->initialize_sdl2_img()) {
        std::cerr << "Failed to initialize SDL2 image!" << std::endl;
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

    if (!sdl2->initialize_sdl2_events()) {
        std::cerr << "Failed to initialize SDL2 inputs! -> EXIT" << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!sdl2->initialize_sdl2_audio()) {
        std::cerr << "Failed to initialize SDL2 audio! -> EXIT" << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!sdl2->initialize_sdl2_ttf()) {
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

    WIN_SIZE sizes = sdl2->get_current_window_size(*win.get_window());
    sdl2->set_window_size(sizes);
    rend.set_font_draw_limit(sizes.HEIGHT);
    fonts.set_char_limit(sizes.WIDTH);

    err =
        files.fill_directories(pathing.get_src_path(), pathing.return_slash());
    if (!err) {
        std::cout << "No directories" << std::endl;
    }

    fonts.create_dir_text(*files.retrieve_directories(), *rend.get_renderer(),
                          *themes.get_text(), fonts.get_font_ptr(),
                          rend.get_font_draw_limit());

    fonts.create_settings_text(*themes.get_text(), fonts.get_font_ptr(),
                               *rend.get_renderer(), fft->get_settings());

    fonts.create_float_number_text(*themes.get_text(), fonts.get_font_ptr(),
                                   *rend.get_renderer());

    fonts.create_integer_number_text(*themes.get_text(), fonts.get_font_ptr(),
                                     *rend.get_renderer());

    const int ticks_per_frame = (1000.0 / 60);
    uint64_t frame_start;
    int frame_time;

    sdl2->set_play_state(true);
    sdl2->set_current_user_state(AT_DIRECTORIES);

    int mouse_x = 0;
    int mouse_y = 0;

    // Not using a mouse mask for anything, so not using the return
    SDL_GetMouseState(&mouse_x, &mouse_y);
    Mouse mouse = { mouse_x, mouse_y, false };

    ThreadData fft_thread;
    fft_thread.thread_ptr = NULL;
    fft_thread.is_ready = false;
    fft_thread.is_running = true;
    fft_thread.m = SDL_CreateMutex();
    fft_thread.c = SDL_CreateCond();

    fft_thread.thread_ptr = SDL_CreateThread(FFT_THREAD, "FFT_THREAD", &fft_thread);
    if(fft_thread.thread_ptr == NULL){
        std::cout<<"Could not create thread! -> " << SDL_GetError() << std::endl;
        return - 1;
    }

   // ThreadTestData testdata;

    //testdata.running = 1;
    //testdata.c = SDL_CreateCond();
    //testdata.m = SDL_CreateMutex();

    //SDL_Thread *thread_ptr = SDL_CreateThread(test_thread, "THREAD_TEST", &testdata);
    //if(thread_ptr == NULL){
     //   std::cerr << "Could not create thread! -> " << SDL_GetError() << std::endl;
      //  return -1;
    //}

    //SDL_LockMutex(testdata.m);
    //testdata.running = 0;
    //SDL_CondSignal(testdata.c);
    //SDL_UnlockMutex(testdata.m);

    //int status;
    //SDL_WaitThread(thread_ptr, &status);

    while (sdl2->get_play_state()) {
        rend.render_bg(themes.get_background());
        rend.render_clear();

        SDL_Event e;

        int drag_start_x;
        int drag_start_y;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            default: {
                break;
            }

            case MOUSEBTN_DOWN: {
                if (e.button.button == MOUSE_LEFT) {
                    mouse.held = true;
                    drag_start_x = e.button.x;
                    drag_start_y = e.button.y;
                }
                break;
            }

            case MOUSEBTN_UP: {
                if (e.button.button == MOUSE_LEFT) {
                    mouse.held = false;
                }
                break;
            }

            case MOUSE_MOVE: {
                if (mouse.held) {
                    int win_x, win_y;
                    SDL_GetWindowPosition(*win.get_window(), &win_x, &win_y);
                    SDL_SetWindowPosition(*win.get_window(),
                                          win_x + e.motion.x - drag_start_x,
                                          win_y + e.motion.y - drag_start_y);
                }
                break;
            }

            case SDL_WINDOWEVENT: {
                if (!mouse.held) {
                    handle_window_event(e.window.event, &std, &sdl2_w);
                }
                break;
            }

            case SDL_KEYDOWN: {

                keydown_handle_state(sdl2->get_current_user_state(),
                                     e.key.keysym, &std, &sdl2_w, userdata);
                break;
            }

            case SDL_KEYUP: {
                if (e.key.keysym.sym == Q) {
                    sdl2->set_play_state(false);
                }

                if (e.key.keysym.sym == T) {
                    if (*win.get_border_bool()) {
                        SDL_SetWindowBordered(*win.get_window(), SDL_FALSE);
                        win.set_border_bool(!*win.get_border_bool());
                    } else {
                        SDL_SetWindowBordered(*win.get_window(), SDL_TRUE);
                        SDL_SetWindowResizable(*win.get_window(), SDL_TRUE);
                        win.set_border_bool(!*win.get_border_bool());
                    }
                }
                break;
            }

            case SDL_QUIT: {
                sdl2->set_play_state(false);
                break;
            }
            }
        }

        switch (*sdl2_ad.get_stream_flag()) {
        default: {
            break;
        }
        case PLAYING: {
            switch (*sdl2_ad.get_next_song_flag()) {
            case WAITING: {
                fft->generate_visual(ad->get_audio_data());
                break;
            }

            case NEXT: {
                goto_next_song(&sdl2_w, &std, userdata);
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        }

        switch (sdl2->get_current_user_state()) {
        case AT_DIRECTORIES: {
            if (fonts.get_dir_vec_size() > 0) {
                rend.render_set_text(sdl2->get_stored_window_size(),
                                     fonts.retrieve_indexed_dir_textvector(
                                         *key.get_vdir_index()));
                rend.render_draw_text(fonts.retrieve_indexed_dir_textvector(
                    *key.get_vdir_index()));
                rend.render_set_text_bg(sdl2->get_stored_window_size(),
                                        fonts.retrieve_indexed_dir_textvector(
                                            *key.get_vdir_index()),
                                        key.get_vdir_cursor_index());
                rend.render_draw_text_bg(themes.get_textbg());
            }
            break;
        }

        case AT_SONGS: {
            if (fonts.get_song_vec_size() > 0) {
                rend.render_set_text(sdl2->get_stored_window_size(),
                                     fonts.retrieve_indexed_song_textvector(
                                         *key.get_vsong_index()));
                rend.render_draw_text(fonts.retrieve_indexed_song_textvector(
                    *key.get_vsong_index()));
                rend.render_set_text_bg(sdl2->get_stored_window_size(),
                                        fonts.retrieve_indexed_song_textvector(
                                            *key.get_vsong_index()),
                                        key.get_vsong_cursor_index());
                rend.render_draw_text_bg(themes.get_textbg());
            }
            break;
        }

        case LISTENING: {
            const WIN_SIZE *sizes = sdl2->get_stored_window_size();
            rend.render_set_bars(&fft->get_data()->output_len, &sizes->HEIGHT,
                                 &sizes->WIDTH, fft->get_bufs()->smear,
                                 fft->get_bufs()->smoothed,
                                 fft->get_bufs()->processed_phases);
            rend.render_draw_bars(&fft->get_data()->output_len,
                                  themes.get_primary(), themes.get_secondary(),
                                  fft->get_bufs()->processed_phases);
            break;
        }

        case AT_SETTINGS: {
            switch (*rend.get_setting_render_mode()) {
            default: {
                break;
            }

            case FLOATS: {
                rend.render_draw_float_settings(
                    fonts.get_float_settings_vec(),
                    sdl2->get_stored_window_size(), themes.get_primary(),
                    themes.get_primary(), key.get_settings_cursor());
                break;
            }

            case INTS: {
                rend.render_draw_int_settings(
                    fonts.get_int_settings_vec(),
                    sdl2->get_stored_window_size(), themes.get_primary(),
                    themes.get_primary(), key.get_settings_cursor());
                break;
            }
            }
        }

        default: {
            break;
        }
        }

        frame_start = SDL_GetTicks64();

        frame_time = SDL_GetTicks64() - frame_start;
        if (ticks_per_frame > frame_time) {
            SDL_Delay(ticks_per_frame - frame_time);
        }

        rend.render_present();
    }


    SDL_LockMutex(fft_thread.m);
    fft_thread.is_ready = true;
    fft_thread.is_running = false;
    SDL_CondSignal(fft_thread.c);
    SDL_UnlockMutex(fft_thread.m);

    int status;
    SDL_WaitThread(fft_thread.thread_ptr, &status);

    fonts.destroy_allocated_fonts();

    free(ad->get_audio_data()->buffer);
    delete ad->get_audio_data();
    delete userdata;
    delete ad;
    delete sdl2;
    delete fft;

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
