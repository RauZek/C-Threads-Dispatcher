#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_SIZE_QUEUE 1024

typedef struct {
    uint32_t queue[MAX_SIZE_QUEUE];
    size_t front, rear, counter;
} queue_t;

void init_queue(queue_t* q);
bool is_queue_empty(queue_t* q);
bool is_queue_full(queue_t* q);
void queue_push(queue_t* q, uint32_t value);
uint32_t queue_pop(queue_t* q);
uint32_t queue_peek(queue_t* q);
void destroy_queue(queue_t* q);

#endif // __QUEUE_H__
