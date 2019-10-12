#pragma once

#include "types.h"

#include <pthread.h>
#include <stdbool.h>


pthread_mutex_t * get_mutex(void);
pthread_cond_t * get_thread_cond(void);

void initialize_threading(void);
/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;
*/

void thread_lock(void);

void thread_unlock(void);

void thread_wait(void);

void thread_signal(void);

bool game_is_paused(const struct controls_thread_data * const data);

bool game_in_progress(const struct controls_thread_data * const data);

