#include <stdio.h>
#include <sys/mman.h>
int main () {
   void *mm;
   int *p = (int*)0x10000000;
   mm = mmap((void*)p, 0x1000, PROT_READ|PROT_WRITE, 
		   MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
   *p = 10;
   printf("p=%p, mm=%p, %d\n", p,mm,*p);
   return 0;
}
