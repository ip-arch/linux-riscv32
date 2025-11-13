#include <unistd.h>
#include <stdio.h>

int main() {
	int fd[2];
	pipe(fd);
	if(fork() == 0) {
		close(fd[0]); // 子は読み取り側を閉じる
		write(fd[1], "Hello Pipe", 10);
	} else {
		close(fd[1]); // 親は書き込み側を閉じる
		char buf[20];
		read(fd[0], buf, 20);
		printf("Parent received: %s\n", buf);
	}
}
