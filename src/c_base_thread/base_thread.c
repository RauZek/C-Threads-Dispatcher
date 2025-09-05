#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "base_thread.h"

void* init(process_fn_t process_fn) {
    base_thread_t* base_thread = calloc(1, sizeof(base_thread_t));

    if (!base_thread) return NULL;

    base_thread->process_fn = process_fn;
    base_thread->instance = base_thread;

    init_queue(&base_thread->queue);

    pthread_mutex_init(&base_thread->mutex, NULL);
    pthread_cond_init(&base_thread->condition, NULL);

    if (pthread_create(&base_thread->th, NULL, loop, base_thread) != 0) {
        destroy_queue(&base_thread->queue);
        pthread_mutex_destroy(&base_thread->mutex);
        pthread_cond_destroy(&base_thread->condition);
        free(base_thread);
    }

    return base_thread;
}

void send_msg(void* instance, uint32_t msg) {
    base_thread_t* base_thread = (base_thread_t*)instance;

    if (!instance) {
        return;
    }

    pthread_mutex_lock(&base_thread->mutex);
    queue_push(&base_thread->queue, msg);
    pthread_mutex_unlock(&base_thread->mutex);
    pthread_cond_signal(&base_thread->condition);
}

void* loop(void* instance) {
    base_thread_t* base_thread = instance;

    while (true) {

        pthread_mutex_lock(&base_thread->mutex);

        while (is_queue_empty(&base_thread->queue)) {
            pthread_cond_wait(&base_thread->condition, &base_thread->mutex);
        }

        uint32_t msg = queue_pop(&base_thread->queue);
        pthread_mutex_unlock(&base_thread->mutex);
        base_thread->process_fn(base_thread->instance, msg);

        if (0 == msg) {
            break;
        }
    }

    return NULL;
}

void destroy(void* instance) {
    base_thread_t* base_thread = instance;

    send_msg(base_thread, 0);
    pthread_join(base_thread->th, NULL);
    destroy_queue(&base_thread->queue);
    pthread_mutex_destroy(&base_thread->mutex);
    pthread_cond_destroy(&base_thread->condition);
    free(base_thread);
}
