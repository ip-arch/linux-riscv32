#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
	int *p, i;
	printf("main is @%10p\n", main);
	for(i=0xF000; i>0; i-=0x1000) {
		munmap((void*)i, 0x1000);
		p=(int*)mmap((void*)i, 0x1000, PROT_WRITE|PROT_READ,
				MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
		printf("@%010p=%010x\n",p, *p);
		fflush(stdout);
	}
	return 0;
}
