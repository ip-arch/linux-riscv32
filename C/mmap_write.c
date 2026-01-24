#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIZE 2

int main(void) {
	size_t length = SIZE * getpagesize();
	char *p;
	int fd = open("test.txt", O_RDWR | O_CREAT, 0644);
	if (fd < 0) {
		perror("open test.txt");
		return 1;
	}

	if (ftruncate(fd, length) < 0) {
                perror("ftruncate");
                close(fd);
                return 1;
        }
	
	p = mmap(NULL, length, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);
	close(fd);
	
	if (p == MAP_FAILED) {
		perror("mmap error");
		return 1;
	}
	for(int i = 0; i < length; i++) {
		if(i%27)
		       	p[i] = (i % 26) + 'A';
		else
		       	p[i] = '\n';
		}
	munmap(p, length);
	printf("all data filled\n");
	return 0;
}
