#ifndef THREADS_H
#define THREADS_H

#include "macro.h"
#include "types.h"
#include <SDL2/SDL_render.h>
#include <pthread.h>
#include <stdio.h>

struct WindowWorker {
  pthread_t*      thread;
  int             cores;
  f32             in_buff[DOUBLE_BUFF];
  f32             out_buff[DOUBLE_BUFF];
  int             start;
  int             end;
  int             paused;
  int             termination_flag;
  int             cycle_complete;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
};

void  calc_hann_window_threads(FourierTransform* FT);
void* hann_window_worker(void* arg);
void  pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  join_thread(pthread_t* context);
void  mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag);
int   create_window_workers(WindowWorker* winwkr, int cores);
void  destroy_window_workers(WindowWorker* winwkr, int cores);
int   instantiate_win_worker(WindowWorker* winwkr, int cores);
#endif
