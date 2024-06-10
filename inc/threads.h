#ifndef THREADS_H
#define THREADS_H

#include "macro.h"
#include "types.h"
#include <SDL2/SDL_render.h>
#include <pthread.h>
#include <stdio.h>

void pause_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void resume_thread(pthread_cond_t* cond, pthread_mutex_t* mutex, int* thread_state);
void join_thread(pthread_t* context);
void mark_for_termination(pthread_cond_t* cond, pthread_mutex_t* mutex, int* flag);
void thread_await(pthread_mutex_t* mutex, pthread_cond_t* cond);
#endif
