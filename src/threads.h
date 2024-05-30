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
};

struct HannThread {
  f32             tmp[DOUBLE_N];
  int             start;
  int             end;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
};

struct LogThread {
  f32             tmp[N];
  f32             start;
  int             end;
  size_t          m;
  f32             max_ampl;
  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             paused;
};
#endif
