#include "audio.h"
#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Well I learned something pretty cool here. However, it's pretty much useless and adds too much
 * overhead for a real time audio visualizer/since it is out of sync most of the time with the main
 * thread. I'll leave it here anyway as I may implement it for the render pipeline and keep the
 * audio processing on a single thread.
 */

void create_threads(ThreadWrapper* t_wrapper) {
  int cores = t_wrapper->cores;
  for (int i = 0; i < cores; i++) {
    create_visualization_renderer(&t_wrapper->rend_context[i], &t_wrapper->ren[i]);
    create_dir_renderer(&t_wrapper->dir_context[i], &t_wrapper->dir[i]);
  }
}

void destroy_threads(ThreadWrapper* t_wrapper) {
  int cores = t_wrapper->cores;
  for (int i = 0; i < cores; i++) {
    mark_for_termination(&t_wrapper->ren[i].cond, &t_wrapper->ren[i].mutex,
                         &t_wrapper->ren[i].termination_flag);
    mark_for_termination(&t_wrapper->dir[i].cond, &t_wrapper->dir[i].mutex,
                         &t_wrapper->dir[i].termination_flag);
    join_thread(&t_wrapper->rend_context[i]);
    join_thread(&t_wrapper->dir_context[i]);
  }
}

void mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag) {
  pthread_mutex_lock(mutex);
  *flag = TRUE;
  pthread_cond_broadcast(cond);
  pthread_mutex_unlock(mutex);
}

void join_thread(pthread_t* context) { pthread_join(*context, NULL); }

void create_visualization_renderer(pthread_t* context, BufRenderThread* Ren) {
  Ren->paused           = TRUE;
  Ren->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  Ren->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  Ren->start            = 0;
  Ren->end              = 0;
  Ren->cycle_complete   = FALSE;
  Ren->termination_flag = FALSE;
  pthread_create(context, NULL, buf_render_worker, Ren);
}

void create_dir_renderer(pthread_t* context, DirFontThread* Dir) {
  Dir->paused           = TRUE;
  Dir->mutex            = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  Dir->cond             = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
  Dir->start            = 0.0f;
  Dir->end              = 0;
  Dir->termination_flag = FALSE;
  Dir->cycle_complete   = FALSE;
  pthread_create(context, NULL, text_render_worker, Dir);
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

void* buf_render_worker(void* arg) {
  BufRenderThread* hann_t = (BufRenderThread*)arg;
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

    if (!hann_t->termination_flag && !hann_t->paused) {
      pause_thread(&hann_t->cond, &hann_t->mutex, &hann_t->paused);
      pthread_mutex_lock(&hann_t->mutex);
      hann_t->cycle_complete = TRUE;
      pthread_mutex_unlock(&hann_t->mutex);
    }
  }

  return NULL;
}

void* text_render_worker(void* arg) {
  DirFontThread* log = (DirFontThread*)arg;
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

    if (!log->termination_flag && !log->paused) {
      pause_thread(&log->cond, &log->mutex, &log->paused);
      pthread_mutex_lock(&log->mutex);
      log->cycle_complete = TRUE;
      pthread_mutex_unlock(&log->mutex);
    }
  }

  return NULL;
}

/*Not using these, but keeping them around for now as a guide*/
// void create_hann_window_th(FourierTransform* FT, ThreadWrapper* TW) {
//
// f32*        fft_in = FT->fft_buffers->fft_in;
// int         cores  = TW->cores;
// HannThread* hann   = TW->hann;
// int         chunk  = N / cores;
//
// for (int i = 0; i < cores; ++i) {
// hann[i].start = (i * chunk);
// hann[i].end   = (i == cores - 1) ? N : (i + 1) * chunk;
// memset(hann[i].win, 0, sizeof(f32) * N);
// memcpy(hann[i].tmp, fft_in, sizeof(f32) * DOUBLE_N);
// resume_thread(&TW->hann[i].cond, &TW->hann[i].mutex, &TW->hann[i].paused);
//}
//
// f32* combined_window = FT->fft_buffers->combined_window;
// int  m               = 0;
// int  m_increm        = 0;
//
// for (int i = 0; i < cores; ++i) {
// while (TRUE) {
// if (hann[i].cycle_complete) {
// break;
//}
//}
// m = hann[i].end;
// memcpy(combined_window + m_increm, hann[i].win, sizeof(f32) * m);
// m_increm += m;
//
// hann[i].cycle_complete = FALSE;
//}
//}
//
// void apply_amp_th(int size, FourierTransform* FT, ThreadWrapper* TW) {
//
// FTransformBuffers* ftbuf = FT->fft_buffers;
// int                cores = TW->cores;
// LogThread*         log   = TW->log;
// int                chunk = size / cores;
//
// for (int i = 0; i < cores; i++) {
// log[i].start = (i == 0) ? (i * chunk) + 1.0f : (i * chunk);
// log[i].end   = (i == cores - 1) ? size : (i + 1) * chunk;
// log[i].m     = 0;
// memset(log[i].tmp_proc, 0, sizeof(f32) * (N / 2));
// memcpy(log[i].tmp, ftbuf->out_raw, sizeof(f32c) * N);
// resume_thread(&TW->log[i].cond, &TW->log[i].mutex, &TW->log[i].paused);
//}
//
// f32*   processed = ftbuf->processed;
// f32*   smoothed  = ftbuf->smoothed;
// size_t m         = 0;
// int    m_increm  = 0;
// f32    max_ampl  = 1.0f;
//
// for (int i = 0; i < cores; i++) {
//// Wait until the worker is finished
// while (TRUE) {
// if (log[i].cycle_complete) {
// break;
//}
//}
//
// m = log[i].m;
// for (int f = 0; f < m; f++) {
// processed[f + m_increm] = log[i].tmp_proc[f];
//}
// m_increm += m;
//
// if (log[i].max_ampl > max_ampl)
// max_ampl = log[i].max_ampl;
//
// log[i].cycle_complete = FALSE;
//}
//
// m = m_increm;
//}
