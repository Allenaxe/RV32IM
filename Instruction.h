#ifndef CPU_INSTRUCTION
#define CPU_INSTRUCTION
#include <stdint.h>
#include <bitset>

namespace RV32IM {
    struct Instruction {
        std::bitset<7> opcode;
        std::bitset<5> rd;
        std::bitset<3> rs1;
        std::bitset<5> rs2;
        std::bitset<3> funct3;
        std::bitset<7> funct7;
        uint32_t imm;
    };
}

#endif