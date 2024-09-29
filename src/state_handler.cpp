#include "../include/audio.hpp"
#include "../include/render_entity.hpp"
#include "../include/fft.hpp"
#include "../include/window_entity.hpp"
#include "../include/events.hpp"
#include "../include/files.hpp"
#include "../include/font_entity.hpp"
#include "../include/switch.hpp"
#include "../include/program_path.hpp"
#include "../include/sdl2_entity.hpp"
#include "../include/theme.hpp"
#include "SDL2/SDL_keycode.h"

void keydown_handle_state(int userstate, SDL_Keysym sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w, USERDATA *userdata) {
    switch (userstate) {
    default: {
        break;
    }

    case AT_DIRECTORIES: {
        directory_keydown_options(sym.sym, std, sdl2_w);
        break;
    }

    case AT_SONGS: {
        song_keydown_options(sym.sym, std, sdl2_w, userdata);
        break;
    }

    case LISTENING: {
        playback_keydown_options(sym.sym, std, sdl2_w);
        break;
    }

    case AT_SETTINGS: {
        settings_keydown_options(sym.sym, sym.mod, std, sdl2_w);
    }
    }
}

void settings_keydown_options(SDL_Keycode sym, uint16_t mod,
                              StdClassWrapper *std, SDL2Wrapper *sdl2_w) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    SDL2KeyInputs *key = sdl2_w->key;
    SDL2Fonts *fonts = sdl2_w->fonts;
    FourierTransform* fft = std->fft;

    switch (sym) {
    default: {
        break;
    }

    case UP: {
        const std::vector<Text> *setting_text_vec = fonts->get_settings_vec();
        const std::string setting_name = (*setting_text_vec)[*key->get_settings_cursor()].name;

        if(setting_name == "Smoothing"){
            const int current = fft->get_settings()->smoothing_amount;
            int mut_current = current;
            mut_current += 1;

            if(mut_current > MAX_SMOOTHING){
                mut_current = MAX_SMOOTHING;
            }

            if(mut_current < MIN_SMOOTHING){
                mut_current = MIN_SMOOTHING;
            }

            fft->set_smoothing(mut_current);
            break;
        }

        if(setting_name == "Smears"){
            const int current = fft->get_settings()->smearing_amount;
            int mut_current = current;
            mut_current += 1;

            if(mut_current > MAX_SMEARS){
                mut_current = MAX_SMEARS;
            }

            if(mut_current < MIN_SMEARS){
                mut_current = MIN_SMEARS;
            }

            fft->set_smear(mut_current);
            break;
        }

        if(setting_name == "Pre-Emphasis"){
            const float current = fft->get_settings()->filter_alpha;
            float mut_current = current;
            mut_current += 0.05;

            if(mut_current > 1.0){
                mut_current = 1.0;
            }

            if(mut_current < 0.0){
                mut_current = 0.0;
            }

            fft->set_alpha(mut_current);
            break;
        }

        if(setting_name == "Highpass Filtering"){
            const float current = fft->get_settings()->filter_coeff;
            float mut_current = current;
            mut_current += 0.05;

            if(mut_current > 1.0){
                mut_current = 1.0;
            }

            if(mut_current < 0.0){
                mut_current = 0.0;
            }

            fft->set_filter(mut_current);
            break;
        }

        break;
    }

    case DOWN: {
        const std::vector<Text> *setting_text_vec = fonts->get_settings_vec();
        const std::string setting_name = (*setting_text_vec)[*key->get_settings_cursor()].name;

        if(setting_name == "Smoothing"){
            const int current = fft->get_settings()->smoothing_amount;
            int mut_current = current;
            mut_current -= 1;

            if(mut_current > MAX_SMOOTHING){
                mut_current = MAX_SMOOTHING;
            }

            if(mut_current < MIN_SMOOTHING){
                mut_current = MIN_SMOOTHING;
            }

            fft->set_smoothing(mut_current);
            break;
        }

        if(setting_name == "Smears"){
            const int current = fft->get_settings()->smearing_amount;
            int mut_current = current;
            mut_current -= 1;

            if(mut_current > MAX_SMEARS){
                mut_current = MAX_SMEARS;
            }

            if(mut_current < MIN_SMEARS){
                mut_current = MIN_SMEARS;
            }

            fft->set_smear(mut_current);
            break;
        }

        if(setting_name == "Pre-Emphasis"){
            const float current = fft->get_settings()->filter_alpha;
            float mut_current = current;
            mut_current -= 0.05;

            if(mut_current > 1.0){
                mut_current = 1.0;
            }

            if(mut_current < 0.0){
                mut_current = 0.0;
            }

            fft->set_alpha(mut_current);
            break;
        }

        if(setting_name == "Highpass Filtering"){
            const float current = fft->get_settings()->filter_coeff;
            float mut_current = current;
            mut_current -= 0.05;

            if(mut_current > 1.0){
                mut_current = 1.0;
            }

            if(mut_current < 0.0){
                mut_current = 0.0;
            }

            fft->set_filter(mut_current);
            break;
        }
        break;
    }

    case LEFT: {
        if (SDL_Keymod(mod & KMOD_SHIFT)) {
            const size_t length = fonts->get_settings_vec_size();
            const size_t *settings_cursor = key->get_settings_cursor();
            int signed_index = static_cast<int>(*settings_cursor);

            signed_index -= 1;
            if (signed_index < 0) {
                signed_index = static_cast<int>(length) - 1;
            }

            key->set_settings_cursor(static_cast<size_t>(signed_index));
            break;
        }

        if (*sdl2_ad->get_stream_flag() == PLAYING ||
            *sdl2_ad->get_stream_flag() == PAUSED) {
            sdl2->set_current_user_state(LISTENING);
        }
        break;
    }

    case RIGHT: {
        if (SDL_Keymod(mod & KMOD_SHIFT)) {
            const size_t length = fonts->get_settings_vec_size();
            const size_t *settings_cursor = key->get_settings_cursor();
            int signed_index = static_cast<int>(*settings_cursor);

            signed_index += 1;
            if (signed_index > static_cast<int>(length) - 1) {
                signed_index = 0;
            }

            key->set_settings_cursor(static_cast<size_t>(signed_index));
            break;
        }
        sdl2->set_current_user_state(AT_DIRECTORIES);
        break;
    }
    }
}

void goto_next_song(SDL2Wrapper *sdl2_w, StdClassWrapper *std,
                    USERDATA *userdata) {

    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    ProgramPath *pathing = std->pathing;
    SDL2KeyInputs *key = sdl2_w->key;
    AudioData *ad = std->ad;
    SDL2Fonts *fonts = sdl2_w->fonts;
    ProgramFiles *files = std->files;

    sdl2_ad->set_flag(WAITING, sdl2_ad->get_next_song_flag());
    sdl2_ad->set_flag(PAUSED, sdl2_ad->get_stream_flag());
    sdl2_ad->pause_audio();
    sdl2_ad->close_audio_device();

    std::string result;
    size_t current_vec_size;
    const size_t *real_song_cursor = key->get_song_cursor_index();
    const size_t *real_svec_index = key->get_song_index();

    size_t ttl_vec_size = fonts->get_song_vec_size();
    if (ttl_vec_size > 0) {
        current_vec_size =
            fonts->retrieve_indexed_song_textvector(*real_svec_index)->size();
        result =
            key->check_cursor_move(current_vec_size, real_song_cursor, "DOWN");
        if (result == "SAFE") {
            key->set_song_cursor_index(*real_song_cursor + 1);
        } else if (result == "MAX") {
            result = fonts->check_vector_index(ttl_vec_size, real_svec_index,
                                               "DOWN");
            if (result == "SAFE") {
                key->set_song_cursor_index(0);
                key->set_song_index(*real_svec_index + 1);
            } else if (result == "MAX") {
                key->set_song_cursor_index(0);
                key->set_song_index(0);
            }
        }
    }

    const size_t font_song_vec_size = fonts->get_song_vec_size();
    const size_t *r_song_vec_index = key->get_song_index();
    const size_t *r_song_cursor = key->get_song_cursor_index();
    const size_t files_size = files->retrieve_directory_files()->size();

    if (font_song_vec_size > 0 && files_size > 0) {
        if (*r_song_vec_index > font_song_vec_size) {
            return;
        }

        std::string filename;
        std::vector<Text> *font_song_vec =
            fonts->retrieve_indexed_song_textvector(*r_song_vec_index);
        filename = key->select_element(font_song_vec, r_song_cursor);
        bool result = false;
        std::string concat_path = pathing->join_str(pathing->get_src_path(),
                                                    pathing->get_opened_dir());
        result = ad->read_audio_file(pathing->join_str(concat_path, filename));

        if (result) {
            key->set_song_index(*r_song_vec_index);
            key->set_song_cursor_index(*r_song_cursor);
            sdl2_ad->set_audio_spec(userdata);
            sdl2_ad->open_audio_device();
            sdl2_ad->resume_audio();
            sdl2_ad->set_flag(PLAYING, sdl2_ad->get_stream_flag());
            sdl2->set_current_user_state(LISTENING);
        }
    }
}

void select_directory(StdClassWrapper *std, SDL2Wrapper *sdl2_w) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    ProgramPath *pathing = std->pathing;
    SDL2KeyInputs *key = sdl2_w->key;
    SDL2Renderer *rend = sdl2_w->rend;
    SDL2Fonts *fonts = sdl2_w->fonts;
    ProgramFiles *files = std->files;
    ProgramThemes *themes = std->themes;

    const size_t font_dir_vec_size = fonts->get_dir_vec_size();
    const size_t *virt_dir_vec_index = key->get_vdir_index();
    const size_t *virt_dir_cursor = key->get_vdir_cursor_index();
    const size_t directories_size = files->retrieve_directories()->size();

    if (font_dir_vec_size > 0 && directories_size > 0) {
        if (*virt_dir_vec_index > font_dir_vec_size) {
            return;
        }

        std::string dirname;
        std::vector<Text> *font_dir_vec =
            fonts->retrieve_indexed_dir_textvector(*virt_dir_vec_index);
        dirname = key->select_element(font_dir_vec, virt_dir_cursor);
        pathing->set_opened_dir(dirname);

        bool result = false;
        std::string concat_path =
            pathing->join_str(pathing->get_src_path(), dirname);

        result = files->fill_files(concat_path, pathing->return_slash());
        if (result) {
            const std::vector<Files> *f = files->retrieve_directory_files();
            if (f->size() > 0) {
                SDL_Renderer *r = *rend->get_renderer();
                SDL_Color *col = themes->get_text();
                TTF_Font *font = fonts->get_font_ptr();
                const size_t *draw_limit = rend->get_font_draw_limit();

                key->set_vsong_cursor_index(0);
                key->set_vsong_index(0);

                fonts->create_file_text(*f, r, *col, font, draw_limit);
                sdl2->set_current_user_state(AT_SONGS);
            }
        }
    }
}

void select_song(StdClassWrapper *std, SDL2Wrapper *sdl2_w,
                 USERDATA *userdata) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    ProgramPath *pathing = std->pathing;
    SDL2KeyInputs *key = sdl2_w->key;
    AudioData *ad = std->ad;
    SDL2Fonts *fonts = sdl2_w->fonts;
    ProgramFiles *files = std->files;

    sdl2_ad->set_flag(PAUSED, sdl2_ad->get_stream_flag());
    sdl2_ad->pause_audio();
    sdl2_ad->close_audio_device();

    const size_t font_song_vec_size = fonts->get_song_vec_size();
    const size_t *virt_song_vec_index = key->get_vsong_index();
    const size_t *virt_song_cursor = key->get_vsong_cursor_index();
    const size_t files_size = files->retrieve_directory_files()->size();

    if (font_song_vec_size > 0 && files_size > 0) {
        if (*virt_song_vec_index > font_song_vec_size) {
            return;
        }

        std::string filename;
        std::vector<Text> *font_song_vec =
            fonts->retrieve_indexed_song_textvector(*virt_song_vec_index);
        filename = key->select_element(font_song_vec, virt_song_cursor);
        bool result = false;
        std::string concat_path = pathing->join_str(pathing->get_src_path(),
                                                    pathing->get_opened_dir());
        result = ad->read_audio_file(pathing->join_str(concat_path, filename));

        if (result) {

            key->set_song_index(*virt_song_vec_index);
            key->set_song_cursor_index(*virt_song_cursor);

            sdl2_ad->set_audio_spec(userdata);
            sdl2_ad->open_audio_device();
            sdl2_ad->resume_audio();
            sdl2_ad->set_flag(PLAYING, sdl2_ad->get_stream_flag());
            sdl2->set_current_user_state(LISTENING);
        }
    }
}

void directory_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                               SDL2Wrapper *sdl2_w) {

    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    SDL2KeyInputs *key = sdl2_w->key;
    SDL2Fonts *fonts = sdl2_w->fonts;
    ProgramFiles *files = std->files;

    switch (sym) {
    default: {
        break;
    }

    case P_KEY: {
        if (*sdl2_ad->get_stream_flag() == PLAYING) {
            std::cout << "Paused" << std::endl;
            sdl2_ad->pause_audio();
            sdl2_ad->set_flag(PAUSED, sdl2_ad->get_stream_flag());
        } else if (*sdl2_ad->get_stream_flag() == PAUSED) {
            std::cout << "Playing" << std::endl;
            sdl2_ad->resume_audio();
            sdl2_ad->set_flag(PLAYING, sdl2_ad->get_stream_flag());
        }
        break;
    }

    case DOWN: {
        std::string result;
        size_t current_vec_size;
        const size_t *virtual_dir_cursor = key->get_vdir_cursor_index();
        const size_t *virtual_dvec_index = key->get_vdir_index();

        size_t ttl_vec_size = fonts->get_dir_vec_size();
        if (ttl_vec_size > 0) {
            current_vec_size =
                fonts->retrieve_indexed_dir_textvector(*virtual_dvec_index)
                    ->size();
            result = key->check_cursor_move(current_vec_size,
                                            virtual_dir_cursor, "DOWN");
            if (result == "SAFE") {
                key->set_vdir_cursor_index(*virtual_dir_cursor + 1);
            } else if (result == "MAX") {
                result = fonts->check_vector_index(ttl_vec_size,
                                                   virtual_dvec_index, "DOWN");
                if (result == "SAFE") {
                    key->set_vdir_cursor_index(0);
                    key->set_vdir_index(*virtual_dvec_index + 1);
                }
            }
        }
        break;
    }

    case UP: {
        std::string result;
        size_t current_vec_size;
        const size_t *virtual_dir_cursor = key->get_vdir_cursor_index();
        const size_t *virtual_dvec_index = key->get_vdir_index();

        size_t ttl_vec_size = fonts->get_dir_vec_size();
        if (ttl_vec_size > 0) {
            current_vec_size =
                fonts->retrieve_indexed_dir_textvector(*virtual_dvec_index)
                    ->size();
            result = key->check_cursor_move(current_vec_size,
                                            virtual_dir_cursor, "UP");

            if (result == "SAFE") {
                key->set_vdir_cursor_index(*virtual_dir_cursor - 1);
            } else if (result == "MIN") {
                result = fonts->check_vector_index(ttl_vec_size,
                                                   virtual_dvec_index, "UP");
                if (result == "SAFE") {
                    current_vec_size = fonts
                                           ->retrieve_indexed_dir_textvector(
                                               *key->get_vdir_index() - 1)
                                           ->size();
                    if (*virtual_dir_cursor > current_vec_size - 1) {
                        key->set_vdir_cursor_index(current_vec_size - 1);
                    }

                    key->set_vdir_index(*virtual_dvec_index - 1);
                    key->set_vdir_cursor_index(current_vec_size - 1);
                }
            }
        }
        break;
    }

    case LEFT: {
        sdl2->set_current_user_state(AT_SETTINGS);
        break;
    }

    case RIGHT: {
        if (fonts->get_song_vec_size() > 0 &&
            files->retrieve_directory_files()->size() > 0) {
            sdl2->set_current_user_state(AT_SONGS);
        }
        break;
    }

    case SPACE: {
        select_directory(std, sdl2_w);
        break;
    }
    }
}

void song_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                          SDL2Wrapper *sdl2_w, USERDATA *userdata) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    SDL2KeyInputs *key = sdl2_w->key;
    SDL2Fonts *fonts = sdl2_w->fonts;

    switch (sym) {
    default: {
        break;
    }

    case P_KEY: {
        if (*sdl2_ad->get_stream_flag() == PLAYING) {
            std::cout << "Paused" << std::endl;
            sdl2_ad->pause_audio();
            sdl2_ad->set_flag(PAUSED, sdl2_ad->get_stream_flag());
        } else if (*sdl2_ad->get_stream_flag() == PAUSED) {
            std::cout << "Playing" << std::endl;
            sdl2_ad->resume_audio();
            sdl2_ad->set_flag(PLAYING, sdl2_ad->get_stream_flag());
        }
        break;
    }

    case DOWN: {
        std::string result;
        size_t current_vec_size;
        const size_t *virtual_song_cursor = key->get_vsong_cursor_index();
        const size_t *virtual_svec_index = key->get_vsong_index();

        size_t ttl_vec_size = fonts->get_song_vec_size();
        if (ttl_vec_size > 0) {
            current_vec_size =
                fonts->retrieve_indexed_song_textvector(*virtual_svec_index)
                    ->size();
            result = key->check_cursor_move(current_vec_size,
                                            virtual_song_cursor, "DOWN");
            if (result == "SAFE") {
                key->set_vsong_cursor_index(*virtual_song_cursor + 1);
            } else if (result == "MAX") {
                result = fonts->check_vector_index(ttl_vec_size,
                                                   virtual_svec_index, "DOWN");
                if (result == "SAFE") {
                    key->set_vsong_cursor_index(0);
                    key->set_vsong_index(*virtual_svec_index + 1);
                }
            }
        }
        break;
    }

    case UP: {
        std::string result;
        size_t current_vec_size;
        const size_t *virtual_song_cursor = key->get_vsong_cursor_index();
        const size_t *virtual_svec_index = key->get_vsong_index();

        size_t ttl_vec_size = fonts->get_song_vec_size();
        if (ttl_vec_size > 0) {
            current_vec_size =
                fonts->retrieve_indexed_song_textvector(*virtual_svec_index)
                    ->size();
            result = key->check_cursor_move(current_vec_size,
                                            virtual_song_cursor, "UP");
            if (result == "SAFE") {
                key->set_vsong_cursor_index(*virtual_song_cursor - 1);
            } else if (result == "MIN") {
                result = fonts->check_vector_index(ttl_vec_size,
                                                   virtual_svec_index, "UP");
                if (result == "SAFE") {
                    current_vec_size = fonts
                                           ->retrieve_indexed_song_textvector(
                                               *key->get_vsong_index() - 1)
                                           ->size();
                    if (*virtual_song_cursor > current_vec_size - 1) {
                        key->set_vsong_cursor_index(current_vec_size - 1);
                    }

                    key->set_vsong_index(*virtual_svec_index - 1);
                    key->set_vsong_cursor_index(current_vec_size - 1);
                }
            }
        }
        break;
    }

    case LEFT: {
        sdl2->set_current_user_state(AT_DIRECTORIES);
        break;
    }

    case RIGHT: {
        if (*sdl2_ad->get_stream_flag() == PLAYING ||
            *sdl2_ad->get_stream_flag() == PAUSED) {
            sdl2->set_current_user_state(LISTENING);
        }
        break;
    }

    case SPACE: {
        select_song(std, sdl2_w, userdata);
        break;
    }
    }
}

void playback_keydown_options(SDL_Keycode sym, StdClassWrapper *std,
                              SDL2Wrapper *sdl2_w) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Audio *sdl2_ad = sdl2_w->sdl2_ad;
    switch (sym) {
    default: {
        break;
    }

    case P_KEY: {
        if (*sdl2_ad->get_stream_flag() == PLAYING) {
            std::cout << "Paused" << std::endl;
            sdl2_ad->pause_audio();
            sdl2_ad->set_flag(PAUSED, sdl2_ad->get_stream_flag());
        } else if (*sdl2_ad->get_stream_flag() == PAUSED) {
            std::cout << "Playing" << std::endl;
            sdl2_ad->resume_audio();
            sdl2_ad->set_flag(PLAYING, sdl2_ad->get_stream_flag());
        }
        break;
    }

    case LEFT: {
        sdl2->set_current_user_state(AT_SONGS);

        break;
    }

    case RIGHT: {
        sdl2->set_current_user_state(AT_SETTINGS);

        break;
    }
    }
}

void handle_window_event(std::uint8_t event, StdClassWrapper *std,
                         SDL2Wrapper *sdl2_w) {
    SDL2INTERNAL *sdl2 = sdl2_w->sdl2;
    SDL2Window *win = sdl2_w->win;
    SDL2Renderer *rend = sdl2_w->rend;
    SDL2Fonts *fonts = sdl2_w->fonts;
    ProgramFiles *files = std->files;
    ProgramThemes *themes = std->themes;

    switch (event) {
    default: {
        break;
    }
    case SDL_WINDOWEVENT_RESIZED: {
        WIN_SIZE ws = sdl2->get_current_window_size(*win->get_window());
        sdl2->set_window_size(ws);
        rend->set_font_draw_limit(ws.HEIGHT);
        fonts->set_char_limit(ws.WIDTH);

        if (files->retrieve_directory_files()->size() > 0) {
            fonts->create_file_text(*files->retrieve_directory_files(),
                                    *rend->get_renderer(), *themes->get_text(),
                                    fonts->get_font_ptr(),
                                    rend->get_font_draw_limit());
        }

        if (files->retrieve_directories()->size() > 0) {
            fonts->create_dir_text(*files->retrieve_directories(),
                                   *rend->get_renderer(), *themes->get_text(),
                                   fonts->get_font_ptr(),
                                   rend->get_font_draw_limit());
        }

        break;
    }

    case SDL_WINDOWEVENT_SIZE_CHANGED: {
        WIN_SIZE ws = sdl2->get_current_window_size(*win->get_window());
        sdl2->set_window_size(ws);
        rend->set_font_draw_limit(ws.HEIGHT);
        fonts->set_char_limit(ws.WIDTH);

        if (files->retrieve_directory_files()->size() > 0) {
            fonts->create_file_text(*files->retrieve_directory_files(),
                                    *rend->get_renderer(), *themes->get_text(),
                                    fonts->get_font_ptr(),
                                    rend->get_font_draw_limit());
        }

        if (files->retrieve_directories()->size() > 0) {
            fonts->create_dir_text(*files->retrieve_directories(),
                                   *rend->get_renderer(), *themes->get_text(),
                                   fonts->get_font_ptr(),
                                   rend->get_font_draw_limit());
        }

        break;
    }
    }
}