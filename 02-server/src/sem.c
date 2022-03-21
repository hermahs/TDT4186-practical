#include <pthread.h>
#include "sem.h"

typedef struct SEM {
	int val;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} SEM;

SEM *sem_init(int initVal) {
	struct SEM* r;
	r = malloc(sizeof(struct SEM));
	r->val = initVal;
	pthread_mutex_init(&r->mutex, NULL);
	pthread_cond_init(&r->cond, NULL);
	return r;
}

int sem_del(SEM *sem) {
	pthread_mutex_destroy(sem->mutex);
	pthread_cond_destroy(sem->cond);
	free(sem);
	return 0;
}

void P(SEM *sem) {
	pthread_mutex_lock(sem->mutex);
	while(sem->val < 1) { pthread_cond_wait(sem->cond, sem->mutex); }
	sem->val--;
}

void V(SEM *sem) {
	sem->val++;
	pthread_mutex_unlock(sem->mutex);
	pthread_cond_brodcast(sem->cond);
}
