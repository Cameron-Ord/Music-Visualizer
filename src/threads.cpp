#include "../include/threads.hpp"
#include "../include/audio.hpp"
#include "../include/events.hpp"
#include "../include/fft.hpp"
#include "../include/sdl2defs.hpp"
#include <unordered_map>

void lock_mutex(SDL_mutex *m, bool *is_locked) {
  *is_locked = true;
  SDL_LockMutex(m);
}

void unlock_mutex(SDL_mutex *m, bool *is_locked) {
  *is_locked = false;
  SDL_UnlockMutex(m);
}

int FFT_THREAD(void *data) {
  ThreadData *ptr = static_cast<ThreadData *>(data);
  FourierTransform *fptr = static_cast<FourierTransform *>(ptr->arg1);

  while (true) {
    SDL_LockMutex(ptr->m);

    if (!ptr->is_ready && ptr->is_running) {
      SDL_CondWait(ptr->c, ptr->m);
    }

    fptr->generate_visual();
    ptr->is_ready = 0;

    if (!ptr->is_running) {
      break;
    }

    SDL_UnlockMutex(ptr->m);
  }

  return 0;
}

int RENDER_THREAD(void *data) {
  ThreadData *ptr = static_cast<ThreadData *>(data);

  while (true) {
    SDL_LockMutex(ptr->m);

    if (!ptr->is_ready && ptr->is_running) {
      SDL_CondWait(ptr->c, ptr->m);
    }

    if (!ptr->is_running) {
      break;
    }

    if (ptr->is_ready && ptr->is_running) {
      SDL_CondWait(ptr->c, ptr->m);
    }

    SDL_UnlockMutex(ptr->m);
  }

  return 0;
}
