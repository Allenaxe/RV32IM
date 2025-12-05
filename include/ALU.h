#ifndef ALU_H
#define ALU_H

#include <cstdint>
#include <bitset>

#include "Structure.h"

namespace RV32IM {

    class ALU {
        public:
            /********************************************************
             * - AluControl ( 4 bits ) - 3 bits funct3 & 1 bit funct7
            ********************************************************/
            static int32_t Operate(std::bitset<4> p_aluOp, ALU_OP_TYPE control_signal, int32_t p_opA, int32_t p_opB);
            static int32_t OpA(uint32_t PC, uint32_t src1, bool p_Selector);
            static int32_t OpB(uint32_t src2, int32_t imm, bool p_Selector);
            static std::bitset<4> AluControl (uint32_t p_funct3, uint32_t p_funct7);

        private:
            static int32_t Adder(uint32_t src1, uint32_t src2, bool cin, bool &carry);
    };

}

#endif