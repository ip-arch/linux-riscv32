// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define N 10

int main(void) {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	
	unsigned int data[N];
	ssize_t n = read(fd, data, sizeof(data));
	if (n != sizeof(data)) {
		perror("read");
		close(fd);
		return 1;
	}
	close(fd);
	
	printf("Random values:\n");
	for (int i = 0; i < N; i++) {
		printf("%08x ", data[i]);
	}
	printf("\n");
	return 0;
}
