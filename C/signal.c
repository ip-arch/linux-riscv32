// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int sig, siginfo_t *info, void *ucontext) {
	printf("Received signal %d from PID %d with value %d\n",
	sig, info->si_pid, info->si_value.sival_int);
}

int main() {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;     // siginfo付きハンドラを使用
	sa.sa_sigaction = handler;
	sigaction(SIGRTMIN, &sa, NULL);
	
	union sigval val;
	val.sival_int = 123;
	sigqueue(getpid(), SIGRTMIN, val); // 自プロセスに送信
	pause();  // ハンドラ呼び出しを待つ
}
