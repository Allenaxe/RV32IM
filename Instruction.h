#ifndef CPU_INSTRUCTION
#define CPU_INSTRUCTION
#include <stdint.h>

namespace RV32IM {
  struct Instruction {
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct3;
    uint8_t funct7;
    int32_t imm;
  };
}
#endif