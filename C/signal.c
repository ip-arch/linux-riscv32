// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handler(int sig, siginfo_t *info, void *ucontext) {
	// 送信元PIDと付随する値を取得可能
	printf("Received signal %d from PID %d with value %d\n",
	sig, info->si_pid, info->si_value.sival_int);
	// 注: printfはAS-safeではない。実用ではwrite()推奨
}

int main() {
	struct sigaction sa = {0};   // ゼロ初期化;
	sa.sa_flags = SA_SIGINFO;    // 詳細情報付きを有効化
	sa.sa_sigaction = handler;
	sigfillset(&sa.sa_mask);     // ハンドラ実行中は全シグナルをブロック
	sigaction(SIGRTMIN, &sa, NULL);
	
	union sigval val;
	val.sival_int = 123;
	// 自プロセスに値を乗せて送信
	sigqueue(getpid(), SIGRTMIN, val);
	pause();  // ハンドラ呼び出しを待つ
		  // ハンドラ終了後ブロックが解除される
}
