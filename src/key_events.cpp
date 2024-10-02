#include "../include/events.hpp"
#include "../include/internal.hpp"
#include "../include/window.hpp"
#include "../include/switch.hpp"

SDL2KeyInputs::SDL2KeyInputs() {
    cursor_index_dirs = 0;
    cursor_index_songs = 0;
    directories_vec_index = 0;
    songs_vec_index = 0;
    virtual_dvec_index = 0;
    virtual_svec_index = 0;
    virtual_cursor_dindex = 0;
    virtual_cursor_sindex = 0;
    settings_cursor = 0;
}

void SDL2KeyInputs::set_settings_cursor(size_t i) {
    settings_cursor = i;
}
const size_t *SDL2KeyInputs::get_settings_cursor() {
    return &settings_cursor;
}

void SDL2KeyInputs::set_vsong_index(size_t i) {
    virtual_svec_index = i;
}

void SDL2KeyInputs::set_vdir_index(size_t i) {
    virtual_dvec_index = i;
}

void SDL2KeyInputs::set_vsong_cursor_index(size_t i) {
    virtual_cursor_sindex = i;
}

void SDL2KeyInputs::set_vdir_cursor_index(size_t i) {
    virtual_cursor_dindex = i;
}

const size_t *SDL2KeyInputs::get_vdir_index() {
    return &virtual_dvec_index;
}

const size_t *SDL2KeyInputs::get_vsong_index() {
    return &virtual_svec_index;
}

const size_t *SDL2KeyInputs::get_vsong_cursor_index() {
    return &virtual_cursor_sindex;
}

const size_t *SDL2KeyInputs::get_vdir_cursor_index() {
    return &virtual_cursor_dindex;
}

void SDL2KeyInputs::set_song_index(size_t i) {
    songs_vec_index = i;
}

void SDL2KeyInputs::set_dir_index(size_t i) {
    directories_vec_index = i;
}

void SDL2KeyInputs::set_song_cursor_index(size_t i) {
    cursor_index_songs = i;
}

void SDL2KeyInputs::set_dir_cursor_index(size_t i) {
    cursor_index_dirs = i;
}

const size_t *SDL2KeyInputs::get_dir_index() {
    return &directories_vec_index;
}

const size_t *SDL2KeyInputs::get_song_index() {
    return &songs_vec_index;
}

const size_t *SDL2KeyInputs::get_song_cursor_index() {
    return &cursor_index_songs;
}

const size_t *SDL2KeyInputs::get_dir_cursor_index() {
    return &cursor_index_dirs;
}

void SDL2KeyInputs::reset_cursor_index(size_t *cursor_index_ptr) {
    *cursor_index_ptr = 0;
}

std::string SDL2KeyInputs::check_cursor_move(size_t vec_size,
                                             const size_t *cursor_ptr,
                                             std::string direction) {
    int signed_size = static_cast<int>(vec_size);
    int signed_cursor = static_cast<int>(*cursor_ptr);

    if (signed_size - 1 < 0) {
        return "EMPTY";
    }

    if (direction == "UP") {
        signed_cursor -= 1;
        if (signed_cursor < 0) {
            return "MIN";
        }
        return "SAFE";

    } else if (direction == "DOWN") {
        signed_cursor += 1;
        if (signed_cursor > signed_size - 1) {
            return "MAX";
        }
        return "SAFE";
    }

    return "INVALID";
}

std::string SDL2KeyInputs::select_element(const std::vector<Text> *d,
                                          const size_t *cursor_index_ptr) {
    return (*d)[*cursor_index_ptr].name;
}

void SDL2KeyInputs::set_mouse_grab(int status){
    no_mouse_grab = status;
}

void SDL2KeyInputs::input_handler(StdClassWrapper *std, SDL2Wrapper *sdl2_w){
        SDL_Event e;
        int mouse_held = 0;
        int drag_start_x;
        int drag_start_y;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            default: {
                break;
            }

            case MOUSEBTN_DOWN: {
                if(!no_mouse_grab){
                    if (e.button.button == MOUSE_LEFT) {
                        mouse_held = 1;
                        drag_start_x = e.button.x;
                        drag_start_y = e.button.y;
                    }
                }
                break;
            }

            case MOUSEBTN_UP: {
                if(!no_mouse_grab){
                    if (e.button.button == MOUSE_LEFT) {
                        mouse_held = 0;
                    }
                }
                break;
            }

            case MOUSE_MOVE: {
                if(!no_mouse_grab){
                    if (mouse_held) {
                        int win_x, win_y;
                        SDL_GetWindowPosition(*sdl2_w->win->get_window(), &win_x, &win_y);
                        SDL_SetWindowPosition(*sdl2_w->win->get_window(),
                                            win_x + e.motion.x - drag_start_x,
                                            win_y + e.motion.y - drag_start_y);
                    }
                }
                break;
            }

            case SDL_WINDOWEVENT: {
                    handle_window_event(e.window.event, std, sdl2_w);
                break;
            }

            case SDL_KEYDOWN: {

                keydown_handle_state(sdl2_w->sdl2->get_current_user_state(),
                                     e.key.keysym, std, sdl2_w);
                break;
            }

            case SDL_KEYUP: {
                if (e.key.keysym.sym == Q) {
                    sdl2_w->sdl2->set_play_state(false);
                }

                if (e.key.keysym.sym == T) {
                    if (*sdl2_w->win->get_border_bool()) {
                        SDL_SetWindowBordered(*sdl2_w->win->get_window(), SDL_FALSE);
                        sdl2_w->win->set_border_bool(!*sdl2_w->win->get_border_bool());
                    } else {
                        SDL_SetWindowBordered(*sdl2_w->win->get_window(), SDL_TRUE);
                        SDL_SetWindowResizable(*sdl2_w->win->get_window(), SDL_TRUE);
                        sdl2_w->win->set_border_bool(!*sdl2_w->win->get_border_bool());
                    }
                }
                break;
            }

            case SDL_QUIT: {
                sdl2_w->sdl2->set_play_state(false);
                break;
            }
            }
        }

}