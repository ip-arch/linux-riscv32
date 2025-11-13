#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void handler(int sig, siginfo_t *si, void *uc) {
	printf("POSIX timer fired (id=%p)\n", i->si_value.sival_ptr);
	}

int main(void) {
	struct sigevent sev;
	struct sigaction sa;
	timer_t timerid;
	struct itimerspec its;
	
	/* シグナルハンドラ設定 */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGRTMIN, &sa, NULL);
	
	/* タイマー生成 */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &timerid;
	timer_create(CLOCK_MONOTONIC, &sev, &timerid);
	
	/* 1秒後に発火し、0.5秒間隔で繰り返す */
	its.it_value.tv_sec = 1;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 500000000;
	timer_settime(timerid, 0, &its, NULL);
	
	while (1) pause();
	return 0;
}
