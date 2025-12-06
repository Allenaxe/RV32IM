#ifndef ALU_H
#define ALU_H

#include <iostream>
#include <cstdint>
#include <bitset>

#include "Structure.h"

namespace RV32IM {

    class ALU {
        public:
            static int32_t Generate_OpA (const uint32_t PC, uint32_t p_Src1, bool p_Selector);
            static int32_t Generate_OpB (uint32_t p_Src2, const int32_t imm, bool p_Selector);
            static int32_t Operate (std::bitset<4> p_ALUFunct, ALU_OP_TYPE p_ALUOp, int32_t p_OpA, int32_t p_OpB);

            // AluControl ( 4 bits ) - 3 bits funct3 & 1 bit funct7
            static std::bitset<4> AluControl (uint32_t p_funct3, uint32_t p_funct7);

        private:
            static int32_t Adder (uint32_t p_Src1, uint32_t p_Src2, bool cin, bool &carry);
    };

}

#endif