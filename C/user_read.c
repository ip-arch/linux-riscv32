#include <stdio.h>
int main () {
   int *p = (int*)0x10000000;
   printf("%d\n", *p);
   return 0;
}
