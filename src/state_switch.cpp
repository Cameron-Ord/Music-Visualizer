#include "../include/audio.hpp"
#include "../include/events.hpp"
#include "../include/fft.hpp"
#include "../include/files.hpp"
#include "../include/font_entity.hpp"
#include "../include/switch.hpp"
#include "../include/program_path.hpp"
#include "../include/sdl2_entity.hpp"
#include "../include/theme.hpp"

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