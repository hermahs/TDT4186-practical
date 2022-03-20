#include "sem.h"

typedef struct SEM {
	int val;
} SEM;

SEM *sem_init(int initVal) {
	struct SEM* r;
	r = malloc(sizeof(struct SEM));
	r->val = initval;
	return r;
}

int sem_del(SEM *sem) {
	free(sem);
	return 0;
}


void P(SEM *sem) {
	while(1) {
		if(sem->val > 0) {
			sem->val--;
			break;
		}
	}
}

void V(SEM *sem) {
	sem->val++;
}
