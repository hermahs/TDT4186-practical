#include <pthread.h>
#include <stdlib.h>
#include "sem.h"

struct SEM {
	int val;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};


SEM *sem_init(int initVal) {
	SEM* r = malloc(sizeof(SEM));
	r->val = initVal;
	pthread_mutex_init(&r->mutex, NULL);
	pthread_cond_init(&r->cond, NULL);
	return r;
}

int sem_del(SEM *sem) {
	pthread_mutex_destroy(&sem->mutex);
	pthread_cond_destroy(&sem->cond);
	free(sem);
	return 0;
}

// Tries to decrement, or blocks
void P(SEM *sem) {
	pthread_mutex_lock(&sem->mutex);
	while(sem->val <= 0) { pthread_cond_wait(&sem->cond, &sem->mutex); }
	sem->val--;
	pthread_mutex_lock(&sem->mutex);
}

// Increments, and informs at least one thread waiting for the condition variable
void V(SEM *sem) {
	pthread_mutex_lock(&sem->mutex);
	sem->val++;
	pthread_cond_signal(&sem->cond);
	pthread_mutex_unlock(&sem->mutex);
}
