#include "threading.h"

#include "types.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;

void initialize_threading(void) {
    if (0 != pthread_mutex_init(&mutex, NULL)) {
        fputs("not able to initialize mutex.", stderr);
        exit(EXIT_FAILURE);
    }

    if (0 == XInitThreads()) {
        fputs("not able to initialize X11 multithreading.", stderr);
        exit(EXIT_FAILURE);
    }
}

void thread_lock(void) {
    if (0 != pthread_mutex_lock(&mutex)) {
        fputs("unable to lock mutex", stderr);
        exit(EXIT_FAILURE);
    }
}

void thread_unlock(void) {
    if (0 != pthread_mutex_unlock(&mutex)) {
        fputs("unable to unlock mutex", stderr);
        exit(EXIT_FAILURE);
    }
}

void thread_wait(void) {
    thread_lock();
    if (0 != pthread_cond_wait(&thread_cond, &mutex)) {
        fputs("unable to wait for thread", stderr);
        exit(EXIT_FAILURE);
    }
    thread_unlock();
}

void thread_signal(void) {
    thread_lock();
    if (0 != pthread_cond_signal(&thread_cond)) {
        fputs("unable to send thread signal", stderr);
        exit(EXIT_FAILURE);
    }
    thread_unlock();
}

bool game_is_paused(const struct controls_thread_data * const data) {
    thread_lock();
    const bool b = data->paused;
    thread_unlock();
    return b;
}

bool game_in_progress(const struct controls_thread_data * const data) {
    thread_lock();
    const bool b = ! data->game_over;
    thread_unlock();
    return b;
}
