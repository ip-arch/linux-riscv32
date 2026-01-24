#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include "rdtime.h"
int main () {
   void *mm;
   uint64_t start_time, end_time;
   int *p = (int*)0x10000000, tmp;
   mm = mmap((void*)p, 0x1000, PROT_READ|PROT_WRITE, 
		   MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
// 1st access
   start_time=read_time_rv32();
   *p = 10;
   // tmp = *p;
   end_time=read_time_rv32();
   printf("1st tick = %lld\n", (end_time - start_time));
// 2nd access
   start_time=read_time_rv32();
   *p = 20;
   // tmp = *p;
   end_time=read_time_rv32();
   printf("2nd tick = %lld\n", (end_time - start_time));
   return 0;
}
