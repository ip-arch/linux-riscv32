#include <stdio.h>
#include <stdint.h>
#include "rdtime.h"

#define ARRAYSIZE 1000

int A[ARRAYSIZE][ARRAYSIZE], B[ARRAYSIZE][ARRAYSIZE];

int main () {
  uint64_t start_time, end_time;
  int i,j;
  // initialize
  for(i=0; i<ARRAYSIZE; i++)
	  for(j=0; j<ARRAYSIZE; j++)
		  A[i][j] = B[i][j] = i+j;
  // i->j loop
  start_time=read_time_rv32();
  for(i=0; i<ARRAYSIZE; i++)
	  for(j=0; j<ARRAYSIZE; j++)
		  A[i][j] += B[i][j]+ i+j;
  end_time=read_time_rv32();
  printf("i->j loop is %lld ticks\n", end_time - start_time);

  // j->i loop
  start_time=read_time_rv32();
  for(j=0; j<ARRAYSIZE; j++)
	  for(i=0; i<ARRAYSIZE; i++)
		  A[i][j] += B[i][j]+ i+j;
  end_time=read_time_rv32();
  printf("j->i loop is %lld ticks\n", end_time - start_time);
  return 0;
}


