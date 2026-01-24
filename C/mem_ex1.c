#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

char inst[16];

int main() {
  int res;
  
  printf("=== RISC-V 32-bit function Demo ===\n\n");
  
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
  
  return 0;
}

