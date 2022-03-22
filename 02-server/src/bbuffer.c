#include <stdlib.h>
#include "bbuffer.h"
#include "sem.h"

typedef struct BNDBUF {
	int* buffer;
	int head;
	int tail;
	unsigned int max;
	unsigned int full;
	unsigned int empty;
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
	int r;	
	// if empty, wait till something has been added to get it
	if (!bb->empty) {
		r = bb->buffer[bb->tail];
		if ( (bb->tail + 1) == bb->head) bb->empty = 1;
		if ( (bb->tail + 1) == bb->max && (bb->tail + 1) != bb->head) bb->tail = 0;
		else bb->tail++;
	} else {
	
	}

	return r;
}

void bb_add(BNDBUF *bb, int fd) {
	// if full, wait till something has been removed before adding it
	if (!bb->full) {
		bb->buffer[bb->head] = fd;
		if ( (bb->head + 1) == bb->tail) bb->full = 1;
		if ( (bb->head + 1) == bb->max && (bb->head + 1) != bb->tail) bb->head = 0;
		else bb->head++;
	} else {
	
	}
}

