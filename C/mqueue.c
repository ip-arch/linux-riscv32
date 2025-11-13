#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
	const char *mq_name = "/myqueue";
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 100;
	attr.mq_curmsgs = 0;
	
	mqd_t mq = mq_open(mq_name, O_CREAT | O_RDWR, 0666, &attr);
	
	if(fork() == 0) { // 子プロセス
		char buf[100];
		mq_receive(mq, buf, 100, NULL);
		printf("Child received: %s\n", buf);
		mq_close(mq);
		exit(0);
	} else { // 親プロセス
		mq_send(mq, "Hello Message Queue", 19, 0);
		mq_close(mq);
		mq_unlink(mq_name);
	}
	return 0;
}
