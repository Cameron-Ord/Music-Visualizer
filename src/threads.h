#ifndef THREADS_H
#define THREADS_H
#include "macro.h"
#include "types.h"
#include <pthread.h>
#include <stdio.h>

struct ThreadWrapper {
  struct HannThread* hann;
  struct LogThread*  log;
  int                cores;
  pthread_t*         log_thread_contexts;
  pthread_t*         hann_thread_contexts;
};

struct HannThread {
  f32             tmp[DOUBLE_N];
  f32             win[N];
  int             start;
  int             end;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
  int             termination_flag;
  int             cycle_complete;
};

struct LogThread {
  f32c            tmp[N];
  f32             tmp_proc[N / 2];
  f32             start;
  int             end;
  size_t          m;
  f32             max_ampl;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
  int             termination_flag;
  int             cycle_complete;
};

void* hann_window_worker(void* arg);
void* log_worker(void* arg);
void  create_threads(ThreadWrapper* t_wrapper);
void  create_hann_thread(pthread_t* context, HannThread* Hann);
void  create_log_thread(pthread_t* context, LogThread* Log);
void  pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  join_thread(pthread_t* context);
void  destroy_threads(ThreadWrapper* t_wrapper);
void  mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag);
void  wait_for_completion(int cycle_state);
void  create_hann_window_th(FourierTransform* FT, ThreadWrapper* TW);
void  apply_amp_th(int size, FourierTransform* FT, ThreadWrapper* TW);
#endif
