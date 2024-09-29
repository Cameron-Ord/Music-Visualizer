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
#include <cstdio>
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
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

    StdClassWrapper std = { .ad = ad,
                            .pathing = &pathing,
                            .files = &files,
                            .fft = fft,
                            .themes = &themes };

    SDL2Wrapper sdl2_w = { .sdl2 = sdl2,
                           .sdl2_ad = &sdl2_ad,
                           .rend = &rend,
                           .win = &win,
                           .key = &key,
                           .fonts = &fonts };

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

    const int ticks_per_frame = (1000.0 / 60);
    uint64_t frame_start;
    int frame_time;

    sdl2->set_play_state(true);
    sdl2->set_current_user_state(AT_DIRECTORIES);

    while (sdl2->get_play_state()) {
        rend.render_bg(*rend.get_renderer(), themes.get_secondary());
        rend.render_clear(*rend.get_renderer());

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
                rend.render_draw_text(*rend.get_renderer(),
                                      fonts.retrieve_indexed_dir_textvector(
                                          *key.get_vdir_index()));
                rend.render_set_text_bg(sdl2->get_stored_window_size(),
                                        fonts.retrieve_indexed_dir_textvector(
                                            *key.get_vdir_index()),
                                        key.get_vdir_cursor_index());
                rend.render_draw_text_bg(*rend.get_renderer(),
                                         themes.get_textbg());
            }
            break;
        }

        case AT_SONGS: {
            if (fonts.get_song_vec_size() > 0) {
                rend.render_set_text(sdl2->get_stored_window_size(),
                                     fonts.retrieve_indexed_song_textvector(
                                         *key.get_vsong_index()));
                rend.render_draw_text(*rend.get_renderer(),
                                      fonts.retrieve_indexed_song_textvector(
                                          *key.get_vsong_index()));
                rend.render_set_text_bg(sdl2->get_stored_window_size(),
                                        fonts.retrieve_indexed_song_textvector(
                                            *key.get_vsong_index()),
                                        key.get_vsong_cursor_index());
                rend.render_draw_text_bg(*rend.get_renderer(),
                                         themes.get_textbg());
            }
            break;
        }

        case LISTENING: {
            const WIN_SIZE *sizes = sdl2->get_stored_window_size();
            rend.render_set_bars(&fft->get_data()->output_len, &sizes->HEIGHT,
                                 &sizes->WIDTH, fft->get_bufs()->smear,
                                 fft->get_bufs()->smoothed);
            rend.render_draw_bars(&fft->get_data()->output_len,
                                  themes.get_primary(), themes.get_textbg(),
                                  *rend.get_renderer());
            break;
        }

        default: {
            break;
        }
        }

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            default: {
                break;
            }

            case SDL_WINDOWEVENT: {
                handle_window_event(e.window.event, &std, &sdl2_w);
                break;
            }

            case SDL_KEYDOWN: {
                keydown_handle_state(sdl2->get_current_user_state(),
                                     e.key.keysym.sym, &std, &sdl2_w, userdata);
                break;
            }

            case SDL_QUIT: {
                sdl2->set_play_state(false);
                break;
            }
            }
        }

        frame_start = SDL_GetTicks64();

        frame_time = SDL_GetTicks64() - frame_start;
        if (ticks_per_frame > frame_time) {
            SDL_Delay(ticks_per_frame - frame_time);
        }

        rend.render_present(*rend.get_renderer());
    }

    fonts.destroy_allocated_fonts();

    delete userdata;
    free(ad->get_audio_data()->buffer);
    delete ad->get_audio_data();
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
