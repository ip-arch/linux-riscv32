// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

#define SEM_NAME "/test_sem"
#define SHARE_NAME "/test_share"

// ==========================================================
// main 関数
// ==========================================================
//

#define BUF_SIZE 4096
struct mybuf {
	size_t count;
	char buf[BUF_SIZE];
};

int main() {
	sem_t *semid;
	struct mybuf *mb;

	char localstr[SHM_SIZE];
	// 1. セマフォの作成と初期化
	semid = sem_open(SEM_NAME);
	if(!semid) {
		perror("sem_open");
		exit(1);
	}
	
	// 2. 共有メモリの取得
        int fd = shm_open(SHARE_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (fd == -1) {
		perror("shm_open failed");
		exit(1);
	}
	if (ftruncate(fd, sizeof(struct mybuf)) == -1) {
                perror("ftruncate");
		exit(1);
	}

               /* Map the object into the caller's address space. */

               mb = mmap(NULL, sizeof(*mb), PROT_READ | PROT_WRITE,
                           MAP_SHARED, fd, 0);
               if (mb == MAP_FAILED)
                   errExit("mmap");

	
	// 3. ロックを取得 (P操作)
	sem_wait(semid);
	printf("Lock acquired. by %d Writing to shared memory...\n", getpid());
	printf("shared memory was (%s)\n", mb.buf);
	
	// 4. クリティカルセクション: 共有メモリへのアクセス
	sprintf(mb.buf, "Hello Shared Memory (Locked Access by %d)", getpid());
	printf("Wrote: %s\n", mb.buf);
	sleep(2);
	// 5. ロックを解放 (V操作)
	sprintf(localstr,"released %d\n", getpid());
	sem_post(semid);
	write(STDOUT_FILENO,  localstr, strlen(localstr));
	sync();
	
	// 6. 共有メモリのデタッチ
	if (sem_unlink(SEM_NAME) == -1) {
		perror("sem_unlink failed");
		// エラー後も続行
	}
	
	return 0;
}

