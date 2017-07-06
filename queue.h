#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>
#include <stdbool.h>

struct queue;

/*
 * Creates a new queue that holds elements of size "elem_size", of capacity "capacity".
 */
struct queue * queue_create(uint8_t elem_size, uint8_t capacity);

/*
 * Pushes an element into the queue. Does not do bound checking.
 */
void queue_push(struct queue *q, const void *elem);

/*
 * Pops an element from queue.
 */
void queue_pop(struct queue *q, void *elem);

/*
 * Checks whether queue is empty.
 */
bool queue_is_empty(const struct queue *q);

/*
 * Checks whether queue is full.
 */
bool queue_is_full(const struct queue *q);

/*
 * Clear all contents of a queue.
 */
void queue_clear(struct queue *q);

/*
 * Frees memory used by queue.
 */
void queue_destroy(struct queue *q);

#endif
