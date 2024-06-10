#include "../inc/audio.h"
#include "../inc/macro.h"
#include "../inc/threads.h"
#include "../inc/types.h"
#include <complex.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
create_window_workers(WindowWorker* winwkr) {
  for (int i = 0; i < WIN_THREAD_COUNT; i++) {
    int err = instantiate_win_worker(&winwkr[i]);
    if (err < 0) {
      return -1;
    }
  }
  return 0;
}

int
instantiate_win_worker(WindowWorker* winwkr) {
  winwkr->start            = 0;
  winwkr->end              = 0;
  winwkr->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  winwkr->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  winwkr->paused           = TRUE;
  winwkr->termination_flag = FALSE;
  winwkr->cycle_complete   = FALSE;
  memset(winwkr->in_buff, 0, sizeof(f32) * DOUBLE_BUFF);
  memset(winwkr->out_buff, 0, sizeof(f32) * DOUBLE_BUFF);
  pthread_create(&winwkr->thread, NULL, hann_window_worker, winwkr);
  pause_thread(&winwkr->cond, &winwkr->mutex, &winwkr->paused);
  return 0;
}

void
destroy_window_workers(WindowWorker* winwkr) {
  for (int i = 0; i < WIN_THREAD_COUNT; i++) {
    mark_for_termination(&winwkr[i].cond, &winwkr[i].mutex, &winwkr[i].termination_flag);
    join_thread(&winwkr[i].thread);
  }
}

void
mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag) {
  pthread_mutex_lock(mutex);
  *flag = TRUE;
  pthread_cond_broadcast(cond);
  pthread_mutex_unlock(mutex);
}

void
join_thread(pthread_t* context) {
  pthread_join(*context, NULL);
}

void
pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state) {
  pthread_mutex_lock(mutex);
  *thread_state = TRUE;
  pthread_cond_broadcast(cond);
  pthread_mutex_unlock(mutex);
}

void
resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state) {
  pthread_mutex_lock(mutex);
  *thread_state = FALSE;
  pthread_cond_broadcast(cond);
  pthread_mutex_unlock(mutex);
}

void
thread_await(pthread_mutex_t* mutex, pthread_cond_t* cond) {
  pthread_mutex_lock(mutex);
  pthread_cond_wait(cond, mutex);
  pthread_mutex_unlock(mutex);
}

void*
hann_window_worker(void* arg) {
  WindowWorker* hann_t = (WindowWorker*)arg;
  for (;;) {

    if (hann_t->paused) {
      thread_await(&hann_t->mutex, &hann_t->cond);
    }

    if (hann_t->termination_flag) {
      break;
    }

    pthread_mutex_lock(&hann_t->mutex);
    for (int i = hann_t->start; i < hann_t->end; ++i) {
      // hann window to reduce spectral leakage before passing it to FFT
      float Nf   = (float)DOUBLE_BUFF;
      float t    = (float)i / (Nf - 1);
      float hamm = 0.54 - 0.46 * cosf(2 * M_PI * t);
      hann_t->in_buff[i] *= hamm;
      hann_t->out_buff[i] = hann_t->in_buff[i];
    }
    pthread_mutex_unlock(&hann_t->mutex);

    if (!hann_t->paused) {
      thread_await(&hann_t->mutex, &hann_t->cond);
    }

    if (hann_t->termination_flag) {
      break;
    }
  }

  return NULL;
}

void
calc_hann_window_threads(FourierTransform* FT) {
  int chunk = DOUBLE_BUFF / WIN_THREAD_COUNT;

  f32* fft_in = FT->fft_buffers->fft_in;
  f32* cpy    = FT->fft_buffers->in_cpy;

  for (int i = 0; i < WIN_THREAD_COUNT; ++i) {
    FT->window_worker[i].start = (i * chunk);
    FT->window_worker[i].end   = (i + 1) * chunk;

    int start = FT->window_worker[i].start;
    int end   = FT->window_worker[i].end;

    memcpy(FT->window_worker[i].in_buff + start, fft_in + start, sizeof(f32) * chunk);
    resume_thread(&FT->window_worker[i].cond, &FT->window_worker[i].mutex, &FT->window_worker[i].paused);
  }

  for (int i = 0; i < WIN_THREAD_COUNT; ++i) {
    pause_thread(&FT->window_worker[i].cond, &FT->window_worker[i].mutex, &FT->window_worker[i].paused);

    f32* buf   = FT->window_worker[i].out_buff;
    int  start = FT->window_worker[i].start;
    int  end   = FT->window_worker[i].end;

    memcpy(cpy + start, buf + start, sizeof(f32) * chunk);
  }
}
