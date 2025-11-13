#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

void *realtime_task(void *arg) {
	struct sched_param param;
	param.sched_priority = 50;  // 優先度
	if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
		perror("sched_setscheduler");
		return NULL;
	}
	for (int i=0; i<5; i++) {
		printf("[RT] iteration %d\n", i);
		sleep(1);
	}
	return NULL;
}

void *normal_task(void *arg) {
	for (int i=0; i<5; i++) {
		printf("[NORM] iteration %d\n", i);
		sleep(1);
	}
	return NULL;
}

int main(void) {
	pthread_t rt, norm;
	pthread_create(&norm, NULL, normal_task, NULL);
	pthread_create(&rt, NULL, realtime_task, NULL);
	pthread_join(rt, NULL);
	pthread_join(norm, NULL);
	return 0;
}
