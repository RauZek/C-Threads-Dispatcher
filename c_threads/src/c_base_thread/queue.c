#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void init_queue(queue_t* q) {
    q->front = 0;
    q->rear = 0;
    q->counter = 0;
}

bool is_queue_empty(queue_t* q) {
    return q->counter == 0;
}

bool is_queue_full(queue_t* q) {
    return q->counter == MAX_SIZE_QUEUE;
}

void queue_push(queue_t* q, uint32_t value) {
    if (is_queue_full(q)) {
        fprintf(stderr, "Queue is full, cannot push. %u\n", value);
        return;
    }
    q->queue[q->rear] = value;
    q->rear = (q->rear + 1) % MAX_SIZE_QUEUE;
    q->counter++;
}

uint32_t queue_pop(queue_t* q) {
    uint32_t val = 0;
    if (is_queue_empty(q)) {
        fprintf(stderr, "Queue is empty, cannot pop.\n");
        return -1;
    }
    val = q->queue[q->front];
    q->front = (q->front + 1) % MAX_SIZE_QUEUE;
    q->counter--;
    return val;
}

uint32_t queue_peek(queue_t* q) {
    if (is_queue_empty(q)) {
        fprintf(stderr, "Queue is empty, cannot peek.\n");
        return 0;
    }
    return q->queue[q->front];
}

void destroy_queue(queue_t* q) {
    q->front = 0;
    q->rear = 0;
    q->counter = 0;
}
