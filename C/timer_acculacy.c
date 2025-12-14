#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define TIMER CLOCK_MONOTONIC

struct timespec prev;

void handler(int sig) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long delta = (now.tv_sec - prev.tv_sec) * 1000000000L
                 + (now.tv_nsec - prev.tv_nsec);
    printf("Interval: %ld ns\n", delta);
    prev = now;
}

int main(void) {
    timer_t tid;
    struct sigevent sev = {0};
    struct itimerspec its = {0};
    
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    
    signal(SIGRTMIN, handler);
    timer_create(TIMER, &sev, &tid);
    
    clock_gettime(TIMER, &prev);
    
    its.it_value.tv_nsec = 1000000;    // 1ms
    its.it_interval.tv_nsec = 1000000; // 1ms周期
    timer_settime(tid, 0, &its, NULL);
    
    while (1) pause();
    return 0;
}

