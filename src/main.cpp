#include "../include/audio.hpp"
#include "../include/events.hpp"
#include "../include/fft.hpp"
#include "../include/files.hpp"
#include "../include/font_entity.hpp"
#include "../include/switch.hpp"
#include "../include/program_path.hpp"
#include "../include/sdl2_entity.hpp"
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

    WIN_SIZE sizes = sdl2->get_current_window_size(*win.get_window());
    sdl2->set_window_size(sizes);

    rend.set_font_draw_limit(sizes.HEIGHT);
    fonts.set_char_limit(sizes.WIDTH);

    rend.create_renderer(win.get_window(), rend.get_renderer());
    if (*rend.get_renderer() == NULL) {
        std::cerr << "Could not create renderer! -> EXIT" << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(*rend.get_renderer(), SDL_BLENDMODE_BLEND);
    SDL_EnableScreenSaver();

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

    fonts.approximate_size_utf8();

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

    WIN_SIZE ws = sdl2->get_current_window_size(*win.get_window());
    sdl2->set_window_size(ws);
    rend.set_font_draw_limit(ws.HEIGHT);
    fonts.set_char_limit(ws.WIDTH);

    rend.set_dir_index(0);
    rend.set_song_index(0);

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
                sdl2_ad.set_flag(WAITING, sdl2_ad.get_next_song_flag());

                size_t current_index = rend.get_song_index();
                size_t vec_size = fonts.get_song_vec_size();

                if (vec_size > 0) {
                    size_t max_length =
                        fonts.get_song_vec(rend.get_song_index())->size();
                    size_t *cursor_ptr = key.get_song_cursor_index();
                    std::string return_str =
                        key.cycle_down_list(max_length, cursor_ptr);

                    if (return_str == "max_reached") {
                        if (vec_size - 1 > current_index) {
                            rend.set_song_index(current_index + 1);
                            *cursor_ptr = 0;
                        } else {
                            rend.set_song_index(0);
                            *cursor_ptr = 0;
                        }
                    }
                }
                sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());

                sdl2_ad.pause_audio();
                sdl2_ad.close_audio_device();

                const std::vector<Text> *f =
                    fonts.get_song_vec(rend.get_song_index());
                const std::string file_name =
                    key.select_element(f, key.get_song_cursor_index());

                if (file_name != "<empty-vector>") {
                    const std::string dir_path = pathing.join_str(
                        pathing.get_src_path(), pathing.get_opened_dir());
                    const std::string file_path =
                        pathing.join_str(dir_path, file_name);
                    bool result = ad->read_audio_file(file_path);
                    if (result) {
                        sdl2_ad.set_audio_spec(userdata);
                        sdl2_ad.open_audio_device();
                        sdl2_ad.resume_audio();
                        sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
                        sdl2->set_current_user_state(LISTENING);
                    }
                }
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
                                     fonts.get_dir_vec(rend.get_dir_index()));
                rend.render_draw_text(*rend.get_renderer(),
                                      fonts.get_dir_vec(rend.get_dir_index()));
                rend.render_set_text_bg(sdl2->get_stored_window_size(),
                                        fonts.get_dir_vec(rend.get_dir_index()),
                                        key.get_dir_cursor_index());
                rend.render_draw_text_bg(*rend.get_renderer(),
                                         themes.get_textbg());
            }
            break;
        }

        case AT_SONGS: {
            if (fonts.get_song_vec_size() > 0) {
                rend.render_set_text(sdl2->get_stored_window_size(),
                                     fonts.get_song_vec(rend.get_song_index()));
                rend.render_draw_text(
                    *rend.get_renderer(),
                    fonts.get_song_vec(rend.get_song_index()));
                rend.render_set_text_bg(
                    sdl2->get_stored_window_size(),
                    fonts.get_song_vec(rend.get_song_index()),
                    key.get_song_cursor_index());
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
                switch (e.key.keysym.sym) {
                default: {
                    break;
                }

                case P_KEY: {
                    if (*sdl2_ad.get_stream_flag() == PLAYING) {
                        std::cout << "Paused" << std::endl;
                        sdl2_ad.pause_audio();
                        sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
                    } else if (*sdl2_ad.get_stream_flag() == PAUSED) {
                        std::cout << "Playing" << std::endl;
                        sdl2_ad.resume_audio();
                        sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
                    }
                    break;
                }

                case ESCAPE: {
                    switch (sdl2->get_current_user_state()) {
                    case AT_SONGS: {
                        if (*sdl2_ad.get_stream_flag() == PLAYING) {
                            sdl2->set_current_user_state(LISTENING);
                        }
                        break;
                    }

                    case AT_DIRECTORIES: {
                        if (*sdl2_ad.get_stream_flag() == PLAYING) {
                            sdl2->set_current_user_state(LISTENING);
                        }
                        break;
                    }
                    }
                    break;
                }

                case SPACE: {
                    switch (sdl2->get_current_user_state()) {
                    default: {
                        break;
                    }

                    case AT_DIRECTORIES: {
                        if (files.retrieve_directories()->size() > 0 &&
                            fonts.get_dir_vec_size() > 0) {
                            const std::vector<Text> *d =
                                fonts.get_dir_vec(rend.get_dir_index());
                            std::string dir_name = key.select_element(
                                d, key.get_dir_cursor_index());

                            if (dir_name != "<empty-vector>") {
                                pathing.set_opened_dir(dir_name);
                                bool result = files.fill_files(
                                    pathing.join_str(pathing.get_src_path(),
                                                     dir_name),
                                    pathing.return_slash());

                                if (files.retrieve_directory_files()->size() >
                                        0 &&
                                    result) {
                                    key.reset_cursor_index(
                                        key.get_song_cursor_index());
                                    rend.set_song_index(0);
                                    fonts.create_file_text(
                                        *files.retrieve_directory_files(),
                                        *rend.get_renderer(),
                                        *themes.get_text(),
                                        fonts.get_font_ptr(),
                                        rend.get_font_draw_limit());
                                    sdl2->set_current_user_state(AT_SONGS);
                                }
                            }
                        }
                        break;
                    }

                    case AT_SONGS: {
                        sdl2_ad.set_flag(PAUSED, sdl2_ad.get_stream_flag());
                        sdl2_ad.pause_audio();
                        sdl2_ad.close_audio_device();

                        if (files.retrieve_directory_files()->size() > 0 &&
                            fonts.get_song_vec_size() > 0) {
                            const std::string file_name = key.select_element(
                                fonts.get_song_vec(rend.get_song_index()),
                                key.get_song_cursor_index());

                            if (file_name != "<empty-vector>") {
                                const std::string dir_path =
                                    pathing.join_str(pathing.get_src_path(),
                                                     pathing.get_opened_dir());
                                if (ad->read_audio_file(pathing.join_str(
                                        dir_path, file_name))) {
                                    sdl2_ad.set_audio_spec(userdata);
                                    sdl2_ad.open_audio_device();
                                    sdl2_ad.resume_audio();
                                    sdl2_ad.set_flag(PLAYING, sdl2_ad.get_stream_flag());
                                    sdl2->set_current_user_state(LISTENING);
                                }
                            }
                        }

                        break;
                    }
                    }
                    break;
                }

                case LEFT: {
                    switch (sdl2->get_current_user_state()) {
                    default: {
                        break;
                    }
                    case AT_DIRECTORIES: {
                        if (*sdl2_ad.get_stream_flag() == PLAYING ||
                            *sdl2_ad.get_stream_flag() == PAUSED) {
                            sdl2->set_current_user_state(LISTENING);
                        }
                        break;
                    }
                    case AT_SONGS: {
                        sdl2->set_current_user_state(AT_DIRECTORIES);
                        break;
                    }
                    case LISTENING: {
                        sdl2->set_current_user_state(AT_SONGS);
                        break;
                    }
                    }
                    break;
                }

                case RIGHT: {
                    switch (sdl2->get_current_user_state()) {
                    default: {
                        break;
                    }
                    case AT_DIRECTORIES: {
                        if (fonts.get_song_vec_size() > 0 &&
                            files.retrieve_directory_files()->size() > 0) {
                            sdl2->set_current_user_state(AT_SONGS);
                        }
                        break;
                    }
                    case AT_SONGS: {
                        if (*sdl2_ad.get_stream_flag() == PLAYING ||
                            *sdl2_ad.get_stream_flag() == PAUSED) {
                            sdl2->set_current_user_state(LISTENING);
                        }
                        break;
                    }
                    case LISTENING: {
                        sdl2->set_current_user_state(AT_DIRECTORIES);
                        break;
                    }
                    }
                    break;
                }

                case UP: {
                    switch (sdl2->get_current_user_state()) {
                    default: {
                        break;
                    }
                    case AT_DIRECTORIES: {
                        if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
                            size_t current_index = rend.get_dir_index();
                            size_t vec_size = fonts.get_dir_vec_size();

                            if (current_index > 0) {
                                rend.set_dir_index(current_index - 1);
                            }

                            key.reset_cursor_index(key.get_dir_cursor_index());
                        } else {
                            size_t current_index = rend.get_dir_index();
                            size_t vec_size = fonts.get_dir_vec_size();

                            if (vec_size > 0) {
                                size_t max_length =
                                    fonts.get_dir_vec(rend.get_dir_index())
                                        ->size();
                                size_t *cursor_ptr = key.get_dir_cursor_index();

                                std::string return_str =
                                    key.cycle_up_list(max_length, cursor_ptr);

                                if (return_str == "min_reached") {
                                    if (current_index > 0) {
                                        rend.set_dir_index(current_index - 1);
                                        size_t new_max =
                                            fonts
                                                .get_dir_vec(
                                                    rend.get_dir_index())
                                                ->size();
                                        *cursor_ptr = (new_max - 1);
                                    } else {
                                        rend.set_dir_index(vec_size - 1);
                                        size_t new_max =
                                            fonts
                                                .get_dir_vec(
                                                    rend.get_dir_index())
                                                ->size();
                                        *cursor_ptr = (new_max - 1);
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case AT_SONGS: {
                        if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
                            size_t current_index = rend.get_song_index();
                            size_t vec_size = fonts.get_song_vec_size();

                            if (current_index > 0) {
                                rend.set_song_index(current_index - 1);
                            }

                            key.reset_cursor_index(key.get_song_cursor_index());
                        } else {
                            size_t current_index = rend.get_song_index();
                            size_t vec_size = fonts.get_song_vec_size();

                            if (vec_size > 0) {
                                size_t max_length =
                                    fonts.get_song_vec(rend.get_song_index())
                                        ->size();
                                size_t *cursor_ptr =
                                    key.get_song_cursor_index();
                                std::string return_str =
                                    key.cycle_up_list(max_length, cursor_ptr);

                                if (return_str == "min_reached") {
                                    if (current_index > 0) {
                                        rend.set_song_index(current_index - 1);
                                        size_t new_max =
                                            fonts
                                                .get_song_vec(
                                                    rend.get_song_index())
                                                ->size();
                                        *cursor_ptr = (new_max - 1);
                                    } else {
                                        rend.set_song_index(vec_size - 1);
                                        size_t new_max =
                                            fonts
                                                .get_song_vec(
                                                    rend.get_song_index())
                                                ->size();
                                        *cursor_ptr = (new_max - 1);
                                    }
                                }
                            }
                        }
                        break;
                    }
                    }
                    break;
                }
                case DOWN: {
                    switch (sdl2->get_current_user_state()) {
                    default: {
                        break;
                    }
                    case AT_DIRECTORIES: {
                        if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
                            size_t current_index = rend.get_dir_index();
                            size_t vec_size = fonts.get_dir_vec_size();

                            if (vec_size - 1 > current_index) {
                                rend.set_dir_index(current_index + 1);
                            }

                            key.reset_cursor_index(key.get_dir_cursor_index());
                        } else {
                            size_t current_index = rend.get_dir_index();
                            size_t vec_size = fonts.get_dir_vec_size();

                            if (vec_size > 0) {
                                size_t max_length =
                                    fonts.get_dir_vec(rend.get_dir_index())
                                        ->size();
                                size_t *cursor_ptr = key.get_dir_cursor_index();
                                std::string return_str =
                                    key.cycle_down_list(max_length, cursor_ptr);

                                if (return_str == "max_reached") {
                                    if (vec_size - 1 > current_index) {
                                        rend.set_dir_index(current_index + 1);
                                        *cursor_ptr = 0;
                                    } else {
                                        rend.set_dir_index(0);
                                        *cursor_ptr = 0;
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case AT_SONGS: {
                        if (SDL_Keymod(e.key.keysym.mod & KMOD_SHIFT)) {
                            size_t current_index = rend.get_song_index();
                            size_t vec_size = fonts.get_song_vec_size();

                            if (vec_size - 1 > current_index) {
                                rend.set_song_index(current_index + 1);
                            }

                            key.reset_cursor_index(key.get_song_cursor_index());
                        } else {
                            size_t current_index = rend.get_song_index();
                            size_t vec_size = fonts.get_song_vec_size();

                            if (vec_size > 0) {
                                size_t max_length =
                                    fonts.get_song_vec(rend.get_song_index())
                                        ->size();
                                size_t *cursor_ptr =
                                    key.get_song_cursor_index();
                                std::string return_str =
                                    key.cycle_down_list(max_length, cursor_ptr);

                                if (return_str == "max_reached") {
                                    if (vec_size - 1 > current_index) {
                                        rend.set_song_index(current_index + 1);
                                        *cursor_ptr = 0;
                                    } else {
                                        rend.set_song_index(0);
                                        *cursor_ptr = 0;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    }

                    break;
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

        frame_start = SDL_GetTicks64();

        frame_time = SDL_GetTicks64() - frame_start;
        if (ticks_per_frame > frame_time) {
            SDL_Delay(ticks_per_frame - frame_time);
        }

        rend.render_present(*rend.get_renderer());
    }

    fonts.destroy_allocated_fonts();

    free(ad->get_audio_data()->buffer);
    delete ad->get_audio_data();
    delete ad;
    delete userdata;
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
