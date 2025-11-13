#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	int s = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "/tmp/mysock");
	bind(s, (struct sockaddr*)&addr, sizeof(addr));
	listen(s, 5);
	int c = accept(s, NULL, NULL);
	char buf[100];
	read(c, buf, 100);
	printf("Received: %s\n", buf);
	close(c); close(s);
}
