#include "bbuffer.h"
#include "sem.h"

typedef struct BNDBUF {
	int* buffer;
	size_t head;
	size_t tail;
	unsigned int max;
	bool full;
	bool empty;
};

BNDBUF *bb_init(unsigned int size) {
	BNDBUF *r = malloc(sizeof(struct BNDBUF));
	// add errorchecking here
	r->max = size;
	r->buffer = malloc(sizeof(int) * r->max);
	// add errochecking here
	r->head = 0;
	r->tail = 0;
	r->full = 0;
	r->empty = 1;

	return r;
}

void bb_del(BNDBUF *bb) {
	free(bb->buffer);
	free(bb);
	return;
}

int bb_get(BNDBUF *bb) {
	// if empty, wait till something has been added to get it
	if (!bb->empty) {
		
	}
}

void bb_add(BNDBUF *bb, int fd) {
	// if full, wait till something has been removed before adding it
	if (!bb->full) {
		bb->buffer[bb->head] = fd;
		bb->head++;
		if ((bb->head + bb->tail) == bb->max) bb->full = 1;	
	}
}

