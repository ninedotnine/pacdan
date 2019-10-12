#pragma once

#include "types.h"

#include <pthread.h>
#include <stdbool.h>

void initialize_threading(void);

void thread_lock(void);

void thread_unlock(void);

void thread_wait(void);

void thread_signal(void);

bool game_is_paused(const struct controls_thread_data * const data);

bool game_in_progress(const struct controls_thread_data * const data);

