#ifndef THREADS_H
#define THREADS_H
#ifdef __linux__

#include "macro.h"
#include "types.h"
#include <pthread.h>
#include <stdio.h>

struct WindowWorker {
  pthread_t*      thread;
  int             cores;
  f32             in_buff[DOUBLE_N];
  f32             out_buff[N];
  int             start;
  int             end;
  int             paused;
  int             termination_flag;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
};

struct FFTWorker {
  pthread_t*      thread;
  int             cores;
  f32             in_buff[N];
  int             start;
  int             end;
  int             paused;
  int             termination_flag;
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
int   create_fft_workers(FFTWorker* fftwkr, int cores);
int   instantiate_fft_worker(FFTWorker* fftwkr, int cores);
void  destroy_fft_workers(FFTWorker* fftwkr, int cores);
void* fft_worker(void* arg);
#endif
#endif
