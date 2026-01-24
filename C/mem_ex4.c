#include <stdio.h>
#include <sys/mman.h>
#include <sys/personality.h>
#include <unistd.h>

int main() {
  char *inst;
  int res;
  unsigned long old_persona;
  
  printf("=== RISC-V 32-bit READ_IMPLIES_EXEC Demo ===\n\n");
  
  // Set READ_IMPLIES_EXEC personality
  old_persona = personality(0xffffffff);
  personality(old_persona | READ_IMPLIES_EXEC);
  printf("Personality: 0x%lx (READ_IMPLIES_EXEC set)\n", 
         personality(0xffffffff));
  
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
  printf("\nSuccess! READ_IMPLIES_EXEC made it executable.\n");
  
  munmap(inst, 16);
  return 0;
}

