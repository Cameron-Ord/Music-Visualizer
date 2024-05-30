#include "audio.h"
#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_threads(ThreadWrapper* t_wrapper) {
  int cores = t_wrapper->cores;
  for (int i = 0; i < cores; i++) {
    create_hann_thread(&t_wrapper->hann_thread_contexts[i], &t_wrapper->hann[i]);
    create_log_thread(&t_wrapper->log_thread_contexts[i], &t_wrapper->log[i]);
  }
}

void destroy_threads(ThreadWrapper* t_wrapper) {
  int cores = t_wrapper->cores;
  for (int i = 0; i < cores; i++) {
    mark_for_termination(&t_wrapper->hann[i].cond, &t_wrapper->hann[i].mutex,
                         &t_wrapper->hann[i].termination_flag);
    mark_for_termination(&t_wrapper->log[i].cond, &t_wrapper->log[i].mutex,
                         &t_wrapper->log[i].termination_flag);
    join_thread(&t_wrapper->log_thread_contexts[i]);
    join_thread(&t_wrapper->log_thread_contexts[i]);
  }
}

void mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag) {
  pthread_mutex_lock(mutex);
  *flag = TRUE;
  pthread_cond_broadcast(cond);
  pthread_mutex_unlock(mutex);
}

void join_thread(pthread_t* context) { pthread_join(*context, NULL); }

void create_hann_thread(pthread_t* context, HannThread* Hann) {
  Hann->paused           = TRUE;
  Hann->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  Hann->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  Hann->start            = 0;
  Hann->end              = 0;
  Hann->cycle_complete   = FALSE;
  Hann->termination_flag = FALSE;
  memset(Hann->tmp, 0, sizeof(f32) * DOUBLE_N);
  memset(Hann->tmp, 0, sizeof(f32) * N);
  pthread_create(context, NULL, hann_window_worker, Hann);
}

void create_log_thread(pthread_t* context, LogThread* Log) {
  Log->paused           = TRUE;
  Log->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  Log->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  Log->m                = 0;
  Log->max_ampl         = 1.0f;
  Log->start            = 0.0f;
  Log->end              = 0;
  Log->termination_flag = FALSE;
  Log->cycle_complete   = FALSE;
  memset(Log->tmp, 0, sizeof(f32c) * N);
  memset(Log->tmp_proc, 0, sizeof(f32) * (N / 2));
  pthread_create(context, NULL, log_worker, Log);
}

void pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state) {
  pthread_mutex_lock(mutex);
  *thread_state = TRUE;
  pthread_mutex_unlock(mutex);
}

void resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state) {
  pthread_mutex_lock(mutex);
  *thread_state = FALSE;
  pthread_cond_signal(cond);
  pthread_mutex_unlock(mutex);
}

void* hann_window_worker(void* arg) {
  HannThread* hann_t = (HannThread*)arg;
  for (;;) {

    if (hann_t->termination_flag) {
      break;
    }

    pthread_mutex_lock(&hann_t->mutex);
    while (hann_t->paused) {

      if (hann_t->termination_flag) {
        break;
      }

      pthread_cond_wait(&hann_t->cond, &hann_t->mutex);
    }
    pthread_mutex_unlock(&hann_t->mutex);

    f32 Nf     = (float)N;
    f32 TWO_PI = 2.0f * M_PI;

    pthread_mutex_lock(&hann_t->mutex);
    for (int i = hann_t->start; i < hann_t->end; ++i) {
      // hann window to reduce spectral leakage before passing it to FFT
      // Summing left and right channels
      f32 sum        = hann_t->tmp[i * 2] + hann_t->tmp[i * 2 + 1];
      hann_t->win[i] = sum / 2;

      float t    = (float)i / (Nf - 1);
      float hann = 0.5 - 0.5 * cosf(TWO_PI * t);

      hann_t->win[i] *= hann;
    }
    pthread_mutex_unlock(&hann_t->mutex);

    if (!hann_t->termination_flag && !hann_t->paused) {
      pause_thread(&hann_t->cond, &hann_t->mutex, &hann_t->paused);
      pthread_mutex_lock(&hann_t->mutex);
      hann_t->cycle_complete = TRUE;
      pthread_mutex_unlock(&hann_t->mutex);
    }
  }

  return NULL;
}

void* log_worker(void* arg) {
  LogThread* log = (LogThread*)arg;
  for (;;) {

    if (log->termination_flag) {
      break;
    }

    pthread_mutex_lock(&log->mutex);
    while (log->paused) {

      if (log->termination_flag) {
        break;
      }

      pthread_cond_wait(&log->cond, &log->mutex);
    }
    pthread_mutex_unlock(&log->mutex);

    pthread_mutex_lock(&log->mutex);
    float step = 1.06f;
    for (float f = log->start; (size_t)f < log->end; f = ceilf(f * step)) {
      float fs = ceilf(f * step);
      float a  = 0.0f;
      for (size_t q = (size_t)f; q < log->end && q < (size_t)fs; ++q) {
        float b = amp(log->tmp[q]);
        if (b > a)
          a = b;
      }
      if (log->max_ampl < a) {
        log->max_ampl = a;
      }
      log->tmp_proc[log->m++] = a;
    }
    pthread_mutex_unlock(&log->mutex);

    if (!log->termination_flag && !log->paused) {
      pause_thread(&log->cond, &log->mutex, &log->paused);
      pthread_mutex_lock(&log->mutex);
      log->cycle_complete = TRUE;
      pthread_mutex_unlock(&log->mutex);
    }
  }

  return NULL;
}
