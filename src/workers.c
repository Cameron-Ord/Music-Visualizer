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
  Hann->termination_flag = FALSE;
  memset(Hann->tmp, 0, sizeof(f32) * DOUBLE_N);
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
  memset(Log->tmp, 0, sizeof(f32) * N);
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
  HannThread* hann = (HannThread*)arg;
  for (;;) {

    if (hann->termination_flag) {
      break;
    }

    pthread_mutex_lock(&hann->mutex);
    while (hann->paused) {

      if (hann->termination_flag) {
        break;
      }

      pthread_cond_wait(&hann->cond, &hann->mutex);
    }
    pthread_mutex_unlock(&hann->mutex);
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
  }

  return NULL;
  // LogThread* _FT = (LogThread*)arg;
  //
  // float step = 1.06f;
  //
  // FTransformBuffers* ftbuf  = _FT->FT->fft_buffers;
  // FTransformData*    ftdata = _FT->FT->fft_data;
  //
  // memset(_FT->tmp, 0, sizeof(f32) * N);
  //
  // for (float f = _FT->start; (size_t)f < _FT->end; f = ceilf(f * step)) {
  // float fs = ceilf(f * step);
  // float a  = 0.0f;
  // for (size_t q = (size_t)f; q < _FT->end && q < (size_t)fs; ++q) {
  // float b = 0.0f;
  //
  // b = amp(ftbuf->out_raw[q]);
  //
  // if (b > a)
  // a = b;
  //}
  // if (_FT->max_ampl < a) {
  //_FT->max_ampl = a;
  //}
  //_FT->tmp[_FT->m++] = a;
  //}
}

// pthread_t threads[FT->cpu_cores];
// LogThread log_thread[FT->cpu_cores];
// int       chunk = size / FT->cpu_cores;
//
// for (int t = 0; t < FT->cpu_cores; ++t) {
// log_thread[t].FT       = FT;
// f32 start              = (t == 0) ? (t * chunk) + 1.0f : (t * chunk);
// log_thread[t].start    = start;
// log_thread[t].m        = 0;
// log_thread[t].max_ampl = 1.0f;
// int end                = (t == FT->cpu_cores - 1) ? size : (t + 1) * chunk;
// log_thread[t].end      = end;
// pthread_create(&threads[t], NULL, log_worker, &log_thread[t]);
//}
//
//
// size_t tmp_m    = 0;
// int    increm   = 0;
// f32    max_ampl = 1.0f;
//
// for (int t = 0; t < FT->cpu_cores; ++t) {
// pthread_join(threads[t], NULL);
// tmp_m = log_thread[t].m;
// memcpy(out_log + increm, log_thread[t].tmp, sizeof(float) * tmp_m);
// increm += tmp_m;
// if (log_thread[t].max_ampl > max_ampl) {
// max_ampl = log_thread[t].max_ampl;
//}
//}

// pthread_t  threads[FT->cpu_cores];
// HannThread hann_thread[FT->cpu_cores];
// int        chunk = N / FT->cpu_cores;
//
// for (int t = 0; t < FT->cpu_cores; ++t) {
// hann_thread[t].FT    = FT;
// hann_thread[t].start = t * chunk;
//
// memcpy(hann_thread[t].tmp, fft_in, sizeof(f32) * DOUBLE_N);
//
// int end = (t == FT->cpu_cores - 1) ? N : (t + 1) * chunk;
//
// hann_thread[t].end = end;
//
// pthread_create(&threads[t], NULL, hann_window_worker, &hann_thread[t]);
//}
//
// for (int t = 0; t < FT->cpu_cores; ++t) {
// pthread_join(threads[t], NULL);
//}
