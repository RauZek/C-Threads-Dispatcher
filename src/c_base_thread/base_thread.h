#ifndef __BASE_THREAD_H__
#define __BASE_THREAD_H__

#include <pthread.h>
#include <stdint.h>

#include "queue.h"

typedef void (*process_fn_t)(void* instance, uint32_t id);

typedef struct {
    process_fn_t process_fn;
    queue_t queue;
    void* instance;
    pthread_t th;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} base_thread_t;

typedef struct {
    void (*process)(void* instance, uint32_t msg);
    void (*get_supported_msg)(uint32_t** msgs, size_t* msg_size);
    uint32_t supported_msgs[22];
    size_t msg_size;
    base_thread_t* thread;
} base_process;

void* init(process_fn_t process_fn);
void send_msg(void* instance, uint32_t msg);
void* loop(void* instance);
void destroy(void* instance);

#endif  //__BASE_THREAD_H__
