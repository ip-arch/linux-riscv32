#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

int main() {
	int shmid = shmget(1234, 1024, IPC_CREAT | 0666);
	char *ptr = (char*)shmat(shmid, NULL, 0);
	strcpy(ptr, "Hello Shared Memory");
	printf("Wrote: %s\n", ptr);
	shmdt(ptr);
}
