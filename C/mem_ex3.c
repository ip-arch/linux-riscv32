#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  char *inst;
  int res;
  
  printf("=== RISC-V 32-bit function Demo 3 ===\n\n");
  
  
  // Map WITHOUT PROT_EXEC
  inst = (char *)mmap(NULL, 16, PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (inst == MAP_FAILED) {
    perror("mmap failed");
    return 1;
  }
  
  printf("Mapped at %p with PROT_READ|PROT_WRITE only\n\n", inst);
  
  // RISC-V 32-bit: li a0, 0x1001; ret
  // li a0, 0x1001 = lui a0, 0x1; addi a0, a0, 0x1
  inst[0] = 0x37;  // lui a0, 0x1
  inst[1] = 0x15;
  inst[2] = 0x00;
  inst[3] = 0x00;
  
  inst[4] = 0x13;  // addi a0, a0, 1
  inst[5] = 0x05;
  inst[6] = 0x15;
  inst[7] = 0x00;
  
  inst[8] = 0x67;  // ret (jalr x0, x1, 0)
  inst[9] = 0x80;
  inst[10] = 0x00;
  inst[11] = 0x00;
  
  printf("Executing code (without explicit PROT_EXEC)...\n");
  res = ((int (*)())inst)();
  printf("Result: %d (0x%x)\n", res, res);
  printf("\nSuccess! \n");
  
  munmap(inst, 16);
  return 0;
}

