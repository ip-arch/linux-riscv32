#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIZE 2

int main(void) {
	size_t length = SIZE * getpagesize();
	char *p;
	int fd = open("/dev/zero", O_RDWR);
	if (fd < 0) {
		perror("open /dev/zero");
		return 1;
	}
	
	p = mmap(NULL, length, PROT_READ | PROT_WRITE,
	MAP_PRIVATE, fd, 0);
	close(fd);
	
	if (p == MAP_FAILED) {
		perror("mmap error");
		return 1;
	}
	for(int i = 0; i < length; i++)
		if(p[i] != 0) {
			fprintf(stderr, "error p[%d]=%d\n", i, p[i]);
			return 1;
		}
	munmap(p, length);
	printf("all data cleared\n");
	return 0;
}
