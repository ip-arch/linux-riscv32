// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <stdio.h>

char buf[256];
int main() {
	int fd;
	int i;
	time_t current;

	fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

	for(i=0; i < 10; i++) {
		current = time(NULL);
		sprintf(buf, "%s PID:%d\n", ctime(&current), getpid());
		write(fd, buf, strlen(buf));
		sleep(1);
	}
	close(fd);
	return 0;
}
