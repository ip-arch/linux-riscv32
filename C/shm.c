// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// セマフォのキーとIDを定義
#define SEM_KEY 5678
#define SHM_KEY 1234
#define SHM_SIZE 1024
int semid;

// セマフォ操作のための共用体 (semctlで使用)
union semun {
	int val;                  /* SETVALのための値 */
	struct semid_ds *buf;     /* IPC_STAT, IPC_SETのためのバッファ */
	unsigned short *array;    /* GETALL, SETALLのための配列 */
	struct seminfo *__buf;    /* IPC_INFOのためのバッファ */
};

// ==========================================================
// セマフォ操作関数
// ==========================================================

// セマフォの初期化または取得
int create_semaphore(int key) {
	// セマフォのセットを作成 (セマフォ数は1)
	int id = semget(key, 1, IPC_CREAT | 0666);
	if (id == -1) {
		perror("semget failed");
		exit(1);
	}
	
	// セマフォが新しく作成された場合、値を1（アンロック状態）に初期化
	union semun arg;
	arg.val = 1;
	if (semctl(id, 0, SETVAL, arg) == -1) {
		perror("semctl SETVAL failed");
		exit(1);
	}
	return id;
}

// セマフォのロック (P操作: 減算)
void lock_semaphore(int id) {
	struct sembuf sb;
	sb.sem_num = 0;   // セマフォ番号 (セット内の最初のセマフォ)
	sb.sem_op = -1;   // 操作: 1減らす (ロック取得)
	sb.sem_flg = SEM_UNDO; // プロセス終了時に自動で解放される設定
	if (semop(id, &sb, 1) == -1) {
		perror("semop lock failed");
		exit(1);
	}
}

// セマフォのアンロック (V操作: 加算)
void unlock_semaphore(int id) {
	struct sembuf sb;
	sb.sem_num = 0;   // セマフォ番号
	sb.sem_op = 1;    // 操作: 1増やす (ロック解放)
	sb.sem_flg = SEM_UNDO;
	if (semop(id, &sb, 1) == -1) {
		perror("semop unlock failed");
		exit(1);
	}
}

// ==========================================================
// main 関数
// ==========================================================

int main() {
	char localstr[SHM_SIZE];
	// 1. セマフォの作成と初期化
	semid = create_semaphore(SEM_KEY);
	printf("Semaphore ID: %d\n", semid);
	
	// 2. 共有メモリの取得
	int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
	if (shmid == -1) {
		perror("shmget failed");
		exit(1);
	}
	char *ptr = (char*)shmat(shmid, NULL, 0);
	if (ptr == (char*)-1) {
		perror("shmat failed");
		exit(1);
	}
	
	// 3. ロックを取得 (P操作)
	lock_semaphore(semid);
	printf("Lock acquired. by %d Writing to shared memory...\n", getpid());
	printf("shared memory was (%s)\n", ptr);
	
	// 4. クリティカルセクション: 共有メモリへのアクセス
	sprintf(ptr, "Hello Shared Memory (Locked Access by %d)", getpid());
	printf("Wrote: %s\n", ptr);
	sleep(2);
	// 5. ロックを解放 (V操作)
	sprintf(localstr,"released %d\n", getpid());
	unlock_semaphore(semid);
	write(STDOUT_FILENO,  localstr, strlen(localstr));
	sync();
	
	// 6. 共有メモリのデタッチ
	if (shmdt(ptr) == -1) {
		perror("shmdt failed");
		// エラー後も続行
	}
	
	return 0;
}

