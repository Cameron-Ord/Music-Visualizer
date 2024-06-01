#include "audio.h"
#include "macro.h"
#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Gonna see if I can get multithreading working on the audio processing for the visuals, with good state
 * management I should be able to make it work without information loss, but we'll see*/

int
create_window_workers(WindowWorker* winwkr, int cores) {
  for (int i = 0; i < cores; i++) {
    int err = instantiate_win_worker(&winwkr[i], cores);
    if (err < 0) {
      return -1;
    }
  }
  return 0;
}

int
instantiate_win_worker(WindowWorker* winwkr, int cores) {
  winwkr->thread           = NULL;
  winwkr->thread           = malloc(sizeof(pthread_t));
  winwkr->cores            = cores;
  winwkr->start            = 0;
  winwkr->end              = 0;
  winwkr->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  winwkr->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  winwkr->paused           = TRUE;
  winwkr->cycle_complete   = FALSE;
  winwkr->termination_flag = FALSE;
  memset(winwkr->out_buff, 0, sizeof(f32) * N);
  memset(winwkr->in_buff, 0, sizeof(f32) * DOUBLE_N);
  pthread_create(winwkr->thread, NULL, hann_window_worker, winwkr);
  pause_thread(&winwkr->cond, &winwkr->mutex, &winwkr->paused, &winwkr->cycle_complete);
  return 0;
}

void
destroy_window_workers(WindowWorker* winwkr, int cores) {
  for (int i = 0; i < cores; i++) {
    mark_for_termination(&winwkr[i].cond, &winwkr[i].mutex, &winwkr[i].termination_flag);
    join_thread(winwkr[i].thread);
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
pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state, int* cycle_complete) {
  pthread_mutex_lock(mutex);
  *thread_state   = TRUE;
  *cycle_complete = TRUE;
  pthread_mutex_unlock(mutex);
}

void
resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state, int* cycle_complete) {
  pthread_mutex_lock(mutex);
  *thread_state   = FALSE;
  *cycle_complete = FALSE;
  pthread_cond_signal(cond);
  pthread_mutex_unlock(mutex);
}
/*I know it's called hann window and im actually using the Hamming window right now, im just too lazy to
 * rename it okay?*/
void*
hann_window_worker(void* arg) {
  WindowWorker* hann_t = (WindowWorker*)arg;
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

    /*All variables are local so mutual exclusions are not necessary*/
    for (int i = hann_t->start; i < hann_t->end; ++i) {
      // hann window to reduce spectral leakage before passing it to FFT
      float Nf   = (float)hann_t->end;
      float t    = (float)i / (Nf - 1);
      float hamm = 0.54 - 0.46 * cosf(2 * M_PI * t);

      /*Accessing interleaved stereo audio*/
      hann_t->in_buff[i * 2] *= hamm;
      hann_t->in_buff[i * 2 + 1] *= hamm;

      /*After both channels are windowed, we sum them and divide the addition by 2*/
      f32 sum             = hann_t->in_buff[i * 2] + hann_t->in_buff[i * 2 + 1];
      hann_t->out_buff[i] = sum / 2;
    }

    if (!hann_t->termination_flag && !hann_t->paused) {
      pause_thread(&hann_t->cond, &hann_t->mutex, &hann_t->paused, &hann_t->cycle_complete);
    }
  }

  return NULL;
}

void
calc_hann_window_threads(FourierTransform* FT) {
  WindowWorker* winwkr = FT->winwkr;
  int           cores  = winwkr->cores;
  int           chunk  = N / cores;
  f32*          fft_in = FT->fft_buffers->fft_in;
  f32*          cpy    = FT->fft_buffers->in_cpy;

  for (int i = 0; i < cores; i++) {
    winwkr[i].start = (i * chunk);
    winwkr[i].end   = (i == cores - 1) ? N : (i + 1) * chunk;
    memcpy(winwkr[i].in_buff, fft_in, sizeof(f32) * DOUBLE_N);
    resume_thread(&winwkr[i].cond, &winwkr[i].mutex, &winwkr[i].paused, &winwkr[i].cycle_complete);
  }

  for (int c = 0; c < cores; c++) {
    while (TRUE) {
      if (winwkr[c].cycle_complete) {
        break;
      }
    }
    int start = winwkr[c].start;
    int end   = winwkr[c].end;

    memcpy(cpy + start, winwkr[c].out_buff + start, sizeof(f32) * end);
  }
}
