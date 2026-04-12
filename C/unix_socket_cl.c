// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	int s = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "/tmp/mysock");
	connect(s, (struct sockaddr*)&addr, sizeof(addr));
	char *buf="Sending from Client";
	write(s, buf, strlen(buf));
	printf("sent: %s\n", buf);
	close(s);
}
