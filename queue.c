#include "queue.h"
#include "mem.h"
#include "osapi.h"
#include "c_types.h"

struct queue {
	uint8_t head;
	uint8_t tail;
	uint8_t size;
	uint8_t elem_size;
	uint8_t capacity;
	void *data;
};

ICACHE_FLASH_ATTR struct queue *queue_create(uint8_t elem_size, uint8_t capacity) {
	struct queue *ptr;
	void *data; 

	ptr = (struct queue *) os_malloc(sizeof(struct queue));
	if (!ptr)
		return NULL;

	data = os_malloc(elem_size * capacity);
	if (!data)
		return NULL;

	ptr->head = 0;
	ptr->tail = 0;
	ptr->size = 0;
	ptr->elem_size = elem_size;
	ptr->capacity = capacity;
	ptr->data = data;

	return ptr;
}

ICACHE_FLASH_ATTR void queue_push(struct queue *q, const void *elem) {
	os_memcpy(q->data + q->head * q->elem_size, elem, q->elem_size);
	q->head = (q->head + 1) % q->capacity;
	q->size++;
}

ICACHE_FLASH_ATTR void queue_pop(struct queue *q, void *elem) {
	os_memcpy(elem, q->data + q->tail * q->elem_size, q->elem_size);
	q->tail = (q->tail + 1) % q->capacity;
	q->size--;
}

ICACHE_FLASH_ATTR bool queue_is_empty(const struct queue *q) {
	return !q->size;
}

ICACHE_FLASH_ATTR bool queue_is_full(const struct queue *q) {
	return q->size == q->capacity;
}

ICACHE_FLASH_ATTR void queue_destroy(struct queue *q) {
	os_free(q->data);
	os_free(q);
}
