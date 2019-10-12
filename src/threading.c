static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t thread_cond = PTHREAD_COND_INITIALIZER;

static void thread_lock(void) {
    if (0 != pthread_mutex_lock(&mutex)) {
        fputs("unable to lock mutex", stderr);
        exit(EXIT_FAILURE);
    }
}

static void thread_unlock(void) {
    if (0 != pthread_mutex_unlock(&mutex)) {
        fputs("unable to unlock mutex", stderr);
        exit(EXIT_FAILURE);
    }
}

static void thread_wait(void) {
    thread_lock();
    if (0 != pthread_cond_wait(&thread_cond, &mutex)) {
        fputs("unable to wait for thread", stderr);
        exit(EXIT_FAILURE);
    }
    thread_unlock();
}

static void thread_signal(void) {
    thread_lock();
    if (0 != pthread_cond_signal(&thread_cond)) {
        fputs("unable to send thread signal", stderr);
        exit(EXIT_FAILURE);
    }
    thread_unlock();
}

static bool game_is_paused(const Controls_thread_data* const data) {
    thread_lock();
    const bool b = data->paused;
    thread_unlock();
    return b;
}

static bool game_in_progress(const Controls_thread_data* const data) {
    thread_lock();
    const bool b = ! data->game_over;
    thread_unlock();
    return b;
}
