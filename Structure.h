#ifndef CPU_STRUCTURE
#define CPU_STRUCTURE
#include <stdint.h>
#include <bitset>

namespace RV32IM {
    // Temporary Useless
    // struct Instruction {
    //     std::bitset<7> opcode;
    //     std::bitset<5> rd;
    //     std::bitset<5> rs1;
    //     std::bitset<5> rs2;
    //     std::bitset<3> funct3;
    //     std::bitset<7> funct7;
    // };

    struct RegisterFileRead {
        uint32_t rs1;
        uint32_t rs2;
    };

    struct DecodeOutput {
        int32_t imm;
        uint32_t rs1;
        uint32_t rs2;
        std::bitset <3> funct3;
        std::bitset <1> funct7;
        std::bitset <10> control_signal;
    };
}

#endif