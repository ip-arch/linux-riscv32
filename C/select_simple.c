#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

int main(void) {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) return 1;
	
	while (1) {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(fd, &rfds);
		int maxfd = fd + 1;
		
		int ret = select(maxfd, &rfds, NULL, NULL, NULL);
		if (ret < 0) break;
		
		if (FD_ISSET(0, &rfds)) {
			char buf[128];
			int n = read(0, buf, sizeof(buf) - 1);
			buf[n] = '\0';
			printf("\n入力: %s", buf);
		}
		
		if (FD_ISSET(fd, &rfds)) {
			unsigned char r[8];
			read(fd, r, 8);
			printf("乱数:");
			for (int i = 0; i < 8; i++) printf(" %02x", r[i]);
			printf("\r");
		}
	}
	return 0;
}
