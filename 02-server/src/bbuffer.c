#include "bbuffer.h"
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include "sem.h"

struct BNDBUF {
	int* buffer;
	size_t size;
	size_t head;
	size_t tail;

	SEM* item_count;
	SEM* free_count;
	pthread_mutex_t mutex;
};

BNDBUF *bb_init(unsigned int size) {
	BNDBUF *r = malloc(sizeof(BNDBUF));
	if(r == NULL)
		error_at_line(-1, errno, __FILE__, __LINE__, "out of memory");

	r->size = size;
	r->buffer = malloc(sizeof(int) * size);
	if(r->buffer == NULL)
		error_at_line(-1, errno, __FILE__, __LINE__, "out memory of");

	r->head = 0;
	r->tail = 0;

	r->item_count = sem_init(0);
	r->free_count = sem_init(size);
	pthread_mutex_init(&r->mutex, NULL);

	return r;
}

void bb_del(BNDBUF *bb) {
	sem_del(bb->item_count);
	sem_del(bb->free_count);
	pthread_mutex_destroy(&bb->mutex);
	free(bb->buffer);
	free(bb);
	return;
}

int bb_get(BNDBUF *bb) {

	P(bb->item_count); // decrease (or block if 0), reserving at least one item for us
	pthread_mutex_lock(&bb->mutex);

	int r = bb->buffer[bb->head];
	bb->head = (bb->head+1) % bb->size;

	pthread_mutex_unlock(&bb->mutex);
	V(bb->free_count); // increase, after the slot is actually read from, to prevent overriding

	return r;
}

void bb_add(BNDBUF *bb, int fd) {

	P(bb->free_count); // decrease free count now, reserving at least one free slot for us
	pthread_mutex_lock(&bb->mutex);

	bb->buffer[bb->tail] = fd;
	bb->tail = (bb->tail+1) % bb->size;

	pthread_mutex_unlock(&bb->mutex);
	V(bb->item_count); // Increase item count /after/ we have stored the value
}

