#ifndef THREADS_HPP
#define THREADS_HPP
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

struct ThreadData {
  SDL_cond *c;
  SDL_mutex *m;
  SDL_Thread *thread_ptr;
  bool is_running;
  bool is_ready;
  bool is_locked;
  void *arg1;
  void *arg2;
  void *arg3;
};

void lock_mutex(SDL_mutex *m, bool *is_locked);
void unlock_mutex(SDL_mutex *m, bool *is_locked);
int FFT_THREAD(void *data);
int RENDER_THREAD(void *data);
#endif