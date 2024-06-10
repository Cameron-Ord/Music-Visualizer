#include "../inc/macro.h"
#include "../inc/threads.h"
#include <complex.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// void*
// hann_window_worker(void* arg) {
// WindowWorker* hann_t = (WindowWorker*)arg;
// for (;;) {
//
// if (hann_t->paused) {
// thread_await(&hann_t->mutex, &hann_t->cond);
//}
//
// if (hann_t->termination_flag) {
// break;
//}
//
// pthread_mutex_lock(&hann_t->mutex);
// for (int i = hann_t->start; i < hann_t->end; ++i) {
//// hann window to reduce spectral leakage before passing it to FFT
// float Nf   = (float)DOUBLE_BUFF;
// float t    = (float)i / (Nf - 1);
// float hamm = 0.54 - 0.46 * cosf(2 * M_PI * t);
//
// hann_t->in_buff[i * 2] *= hamm;
// hann_t->in_buff[i * 2 + 1] *= hamm;
//}
// pthread_mutex_unlock(&hann_t->mutex);
//
// if (!hann_t->paused) {
// thread_await(&hann_t->mutex, &hann_t->cond);
//}
//
// if (hann_t->termination_flag) {
// break;
//}
//}
//
// return NULL;
//}
