#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define N 1024  /* 配列要素数 */

int main(void) {
	int fd = open("/dev/zero", O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	
	size_t length = N * sizeof(int);
	int *array = mmap(NULL, length,
	PROT_READ | PROT_WRITE,
	MAP_PRIVATE, fd, 0);
	close(fd);
	
	if (array == MAP_FAILED) {
		perror("mmap");
		return 1;
	}
	
	/* すべて 0 で初期化されていることを確認 */
	for (int i = 0; i < 10; i++) {
		printf("%d ", array[i]);
	}
	printf("...\n");
	
	munmap(array, length);
	return 0;
}
