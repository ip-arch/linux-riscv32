#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
	const char *fifo_path = "/tmp/myfifo";
	mkfifo(fifo_path, 0666); // FIFO作成
	
	if(fork() == 0) { // 子プロセス
		int fd = open(fifo_path, O_RDONLY);
		char buf[100];
		read(fd, buf, sizeof(buf));
		printf("Child received: %s\n", buf);
		close(fd);
		exit(0);
	} else { // 親プロセス
		int fd = open(fifo_path, O_WRONLY);
		write(fd, "Hello Named Pipe", 17);
		close(fd);
	}
	return 0;
}
