#include "../inc/audio.h"
#include "../inc/font.h"
#include "../inc/input.h"
#include "../inc/music_visualizer.h"
#include "../inc/render.h"

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
      free_all(app);
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
      if (!SDLC->SSPtr->pb_state->playing_song) {
        handle_mouse_wheel(e.wheel.y, SDLC, app->FCPtr);
      }
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
        free_all(app);
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
