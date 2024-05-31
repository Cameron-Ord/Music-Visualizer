#ifndef THREADS_H
#define THREADS_H
#include "macro.h"
#include "types.h"
#include <pthread.h>
#include <stdio.h>

struct BufRenderThread {
  int             start;
  int             end;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
  int             termination_flag;
  int             cycle_complete;
};

struct DirFontThread {
  f32             start;
  int             end;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
  int             termination_flag;
  int             cycle_complete;
};

struct ThreadWrapper {
  struct BufRenderThread* ren;
  struct DirFontThread*   dir;
  int                     cores;
  pthread_t*              dir_context;
  pthread_t*              rend_context;
};

void* buf_render_worker(void* arg);
void* text_render_worker(void* arg);
void  create_threads(ThreadWrapper* t_wrapper);
void  create_visualization_renderer(pthread_t* context, BufRenderThread* Ren);
void  create_dir_renderer(pthread_t* context, DirFontThread* Dir);
void  pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void  join_thread(pthread_t* context);
void  destroy_threads(ThreadWrapper* t_wrapper);
void  mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag);
#endif
