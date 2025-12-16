#ifndef ALU_H
#define ALU_H

#include <iostream>
#include <cstdint>
#include <bitset>
#include <array>

#include "Structure.h"
#include "Exception.h"

namespace RV32IM {

    class ALU {
        public:
            static uint32_t Generate_OpA (const uint32_t PC, uint32_t p_Src1, bool p_Selector);
            static uint32_t Generate_OpB (uint32_t p_Src2, const int32_t imm, bool p_Selector);
            static uint32_t Operate (std::bitset<4> p_ALUFunct, ALU_OP_TYPE p_ALUOp, uint32_t p_OpA, uint32_t p_OpB);

            // AluControl ( 4 bits ) - 3 bits funct3 & 1 bit funct7
            static std::bitset<4> AluControl (uint32_t p_funct3, uint32_t p_funct7);

        private:
            static int32_t Adder (uint32_t p_Src1, uint32_t p_Src2, bool cin, bool &carry);

            // Multiplier
            static uint32_t MUL             (uint32_t a, uint32_t b);
            static uint32_t MULH            ( int32_t a,  int32_t b);
            static uint32_t MULHSU          ( int32_t a, uint32_t b);
            static uint32_t MULHU           (uint32_t a, uint32_t b);

            static uint64_t mul64_signed    (int64_t  a,  int64_t b);
            static uint64_t mul64_mixed     (int64_t  a, uint64_t b);
            static uint64_t mul64_unsigned  (uint64_t a, uint64_t b);

            // --- Hardware blocks ---
            static std::array<int,16> booth_radix4 (int64_t b);
            static std::vector<uint64_t> booth_partial_products (int64_t a, const std::array<int,16>& code);

            // Wallace tree + adders
            static std::pair<uint64_t,uint64_t> csa_64 (uint64_t x, uint64_t y, uint64_t z);
            static uint64_t ripple_adder_64 (uint64_t a, uint64_t b);
            static uint64_t wallace_tree (const std::vector<uint64_t>& pp);

            inline static int64_t sign_extend_32 (int32_t x) { return (int64_t)x; }
            inline static uint64_t mask64 (uint64_t x) { return x & 0xFFFFFFFFFFFFFFFFULL; }

            // Divider
            static uint32_t DIVU    (uint32_t p_OpA, uint32_t p_OpB);
            static uint32_t DIV     ( int32_t p_OpA,  int32_t p_OpB);
            static uint32_t REMU    (uint32_t p_OpA, uint32_t p_OpB);
            static uint32_t REM     ( int32_t p_OpA,  int32_t p_OpB);
    };

}

#endif