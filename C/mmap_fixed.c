#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define MADR (void*)0x10000000

int main() {
  char *prevmap, *newmap;
  int res;
  
  printf("=== RISC-V 32-bit MAP_FIXED Demo ===\n\n");
  
  
  // Map WITHOUT PROT_EXEC
  prevmap = (char *)mmap(MADR, 0x1000, PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (prevmap == MAP_FAILED) {
    perror("prevmap mmap failed");
    return 1;
  }
  sprintf(prevmap, "%s\n", "PREVIOUS MAP DATA");
  
  printf("Prev: Mapped at %p : %s\n", prevmap, prevmap);
  
  newmap = (char *)mmap(prevmap, 0x1000, PROT_WRITE|PROT_READ,
                      MAP_FIXED | MAP_PRIVATE |MAP_ANONYMOUS, -1, 0);
  
  if (newmap == MAP_FAILED) {
    perror("newmap mmap failed");
    return 1;
  }
  printf("New: Mapped at %p : @[0]=0x%02x %s\n", newmap, newmap[0], newmap);
  
  return 0;
}

