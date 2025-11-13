#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

void handler(int sig) {
	printf("Timer fired!\n");
}

int main(void) {
	struct itimerval timer = {
		.it_interval = {0, 500000}, /* 0.5秒ごと */
		.it_value    = {1, 0}       /* 初回は1秒後 */
	};
	
	signal(SIGALRM, handler);
	setitimer(ITIMER_REAL, &timer, NULL);
	
	while (1) pause();
	return 0;
}
