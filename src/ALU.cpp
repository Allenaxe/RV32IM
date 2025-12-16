#include "ALU.h"

namespace RV32IM {

    int32_t ALU::Adder (uint32_t p_Src1, uint32_t p_Src2, bool cin, bool &carry) {
        uint32_t result = 0;
        bool carry_local = cin;

        for(int i = 0; i < 32; i++){
            bool bit1 = (p_Src1 >> i) & 1;
            bool bit2 = (p_Src2 >> i) & 1;
            bool sum, carryOut;

            sum = bit1 ^ bit2 ^ carry_local;
            carryOut = (bit1 & bit2) | (bit1 & carry_local) | (bit2 & carry_local);
            if(sum) {
                result |= (1u << i);
            }
            carry_local = carryOut;
        }

        carry = carry_local;
        return (int32_t)result;
    }

    // ============================================================
    //  Radix-4 Booth recoding (returns 16 booth codes)
    //  code = {-2, -1, 0, +1, +2}
    // ============================================================
    std::array<int,16> ALU::booth_radix4 (int64_t b) {
        std::array<int,16> code{};
        int64_t ext = (b << 1);         // append implicit b[-1] = 0

        for (int i = 0; i < 16; i++) {
            int bits = (ext >> (2*i)) & 0b111; // 3-bit window

            switch(bits) {
                case 0b000: case 0b111: code[i] =  0; break;
                case 0b001: case 0b010: code[i] = +1; break;
                case 0b011:             code[i] = +2; break;
                case 0b100:             code[i] = -2; break;
                case 0b101: case 0b110: code[i] = -1; break;
            }
        }
        return code;
    }

    // ============================================================
    //  Partial product generation (16 partial products)
    // ============================================================
    std::vector<uint64_t> ALU::booth_partial_products (
        int64_t a, const std::array<int,16>& code) {
        std::vector<uint64_t> pp(16);

        for (int i = 0; i < 16; i++) {
            int c = code[i];
            int64_t p = 0;

            switch(c) {
                case 0:  p = 0;         break;
                case +1: p = a;         break;
                case -1: p = -a;        break;
                case +2: p = (a << 1);  break;
                case -2: p = -(a << 1); break;
            }

            // Shift according to the booth group
            uint64_t shifted = static_cast<uint64_t>(p) << (2 * i);
            pp[i] = shifted;
        }
        return pp;
    }

    // ============================================================
    //  64-bit carry-save adder (Process through each bit)
    // ============================================================
    std::pair<uint64_t,uint64_t> ALU::csa_64 (
        uint64_t x, uint64_t y, uint64_t z) {
        uint64_t sum = 0, carry = 0;

        for (int i = 0; i < 64; i++) {
            uint64_t a = (x >> i) & 1ULL;
            uint64_t b = (y >> i) & 1ULL;
            uint64_t c = (z >> i) & 1ULL;

            uint64_t s = a ^ b ^ c;
            uint64_t g = (a & b) | (b & c) | (c & a);

            sum   |= (s << i);
            carry |= (g << i);
        }
        return {sum, carry << 1};
    }

    // ============================================================
    //  64-bit ripple-carry adder (Last CPA)
    // ============================================================
    uint64_t ALU::ripple_adder_64 (uint64_t a, uint64_t b) {
        uint64_t result = 0;
        uint64_t carry  = 0;

        for (int i = 0; i < 64; i++) {
            uint64_t x = (a >> i) & 1ULL;
            uint64_t y = (b >> i) & 1ULL;

            uint64_t s = x ^ y ^ carry;
            uint64_t g = (x & y) | (y & carry) | (carry & x);

            result |= (s << i);
            carry = g;
        }
        return result;
    }

    // ============================================================
    //  Wallace Tree reduction (Classic Wallace 3→2)
    // ============================================================
    uint64_t ALU::wallace_tree (const std::vector<uint64_t>& pp) {
        std::vector<uint64_t> layer = pp;
        while (layer.size() > 2)
        {
            std::vector<uint64_t> next;

            size_t i = 0;
            for (; i + 2 < layer.size(); i += 3) {
                auto [s, c] = csa_64(layer[i], layer[i+1], layer[i+2]);
                next.push_back(s);
                next.push_back(c);
            }
            // remaining 1 or 2 terms
            for (; i < layer.size(); i++)
                next.push_back(layer[i]);

            layer = next;
        }

        // Final CPA to get full 64-bit product
        return ripple_adder_64(layer[0], layer[1]);
    }

    // ============================================================
    //  Top-level signed / unsigned multiply paths
    // ============================================================
    uint64_t ALU::mul64_signed (int64_t a, int64_t b) {
        auto code = booth_radix4(b);
        auto pp   = booth_partial_products(a, code);
        return wallace_tree(pp);
    }

    uint64_t ALU::mul64_mixed (int64_t a, uint64_t b) {
        auto code = booth_radix4(static_cast<int64_t>(b));
        auto pp   = booth_partial_products(a, code);
        return wallace_tree(pp);
    }

    uint64_t ALU::mul64_unsigned(uint64_t a, uint64_t b)
    {
        auto code = booth_radix4(static_cast<int64_t>(b));
        auto pp   = booth_partial_products(static_cast<int64_t>(a), code);
        return wallace_tree(pp);
    }

    // ============================================================
    //  RISC-V public APIs
    // ============================================================
    uint32_t ALU::MUL(uint32_t p_OpA, uint32_t p_OpB)
    {
        uint64_t r = mul64_signed(static_cast<int64_t>(p_OpA), static_cast<int64_t>(p_OpB));
        return (uint32_t)(r & 0xFFFFFFFFULL);
    }

    uint32_t ALU::MULH(int32_t p_OpA, int32_t p_OpB)
    {
        uint64_t r = mul64_signed(p_OpA, p_OpB);
        return (uint32_t)(r >> 32);
    }

    uint32_t ALU::MULHSU(int32_t p_OpA, uint32_t p_OpB)
    {
        uint64_t r = mul64_mixed(p_OpA, p_OpB);
        return (uint32_t)(r >> 32);
    }

    uint32_t ALU::MULHU(uint32_t p_OpA, uint32_t p_OpB)
    {
        uint64_t r = mul64_unsigned(p_OpA, p_OpB);
        return (uint32_t)(r >> 32);
    }

    uint32_t ALU::DIVU(uint32_t p_OpA, uint32_t p_OpB) {
        uint32_t quotient = 0;
        uint32_t remainder = 0;

        for (int i = 31; i >= 0; --i) {
            remainder = (remainder << 1) | ((p_OpA >> i) & 1);
            if (remainder >= p_OpB) {
                remainder -= p_OpB;
                quotient |= (1u << i);
            }
        }
        return quotient;
    }

    uint32_t ALU::DIV(int32_t p_OpA, int32_t p_OpB) {
        bool negQuotient = (p_OpA < 0) ^ (p_OpB < 0);

        uint32_t absDividend = (p_OpA < 0) ? ~static_cast<uint32_t>(p_OpA) + 1 : static_cast<uint32_t>(p_OpA);
        uint32_t absDivisor  = (p_OpB < 0) ? ~static_cast<uint32_t>(p_OpB) + 1 : static_cast<uint32_t>(p_OpB);

        uint32_t quotient = DIVU(absDividend, absDivisor);

        if (negQuotient) quotient = -int32_t(quotient);

        return quotient;
    }

    uint32_t ALU::REMU(uint32_t p_OpA, uint32_t p_OpB) {
        uint32_t quotient = 0;
        uint32_t remainder = 0;

        for (int i = 31; i >= 0; --i) {
            remainder = (remainder << 1) | ((p_OpA >> i) & 1);
            if (remainder >= p_OpB) {
                remainder -= p_OpB;
                quotient |= (1u << i);
            }
        }
        return remainder;
    }

    uint32_t ALU::REM(int32_t p_OpA, int32_t p_OpB) {
        bool negRemainder = (p_OpA < 0);

        uint32_t absDividend = (p_OpA < 0) ? -uint32_t(p_OpA) : uint32_t(p_OpA);
        uint32_t absDivisor  = (p_OpB < 0) ? -uint32_t(p_OpB) : uint32_t(p_OpB);

        uint32_t reminder = REMU(absDividend, absDivisor);

        if (negRemainder) reminder = -int32_t(reminder);

        return reminder;
    }

    // Determine calculate type
    // @Return: bitset<4> {funct3 (3 bit) + funct7 (1 bit)}
    std::bitset<4> ALU::AluControl (uint32_t p_funct3, uint32_t p_funct7) {
        std::bitset<4> type = p_funct3;
        type <<= 1;
        type[0] = (p_funct7 >> 5) & 1;
        return type;
    }

    uint32_t ALU::Generate_OpA (const uint32_t PC, uint32_t p_Src1, bool p_Selector) {
        // signal: Branch / Jump = 1
        return p_Selector ? PC : p_Src1;
    }

    uint32_t ALU::Generate_OpB (uint32_t p_Src2, const int32_t imm, bool p_Selector) {
        // signal: ALUsrc = 1
        return p_Selector ? imm : p_Src2;
    }

    uint32_t ALU::Operate (std::bitset<4> p_ALUFunct,
                          ALU_OP_TYPE p_ALUOp,
                          uint32_t p_OpA,
                          uint32_t p_OpB)
    {
        bool carryOut = false; // Carry Flag
        uint32_t complement;

        switch (p_ALUOp) {
            // Load / Store
            case ALU_OP_TYPE::MEMORY_REF: {
                return p_OpA + p_OpB;
            }

            // LUI
            case ALU_OP_TYPE::LUI: {
                return p_OpB;
            }
            // AUIPC
            case ALU_OP_TYPE::AUIPC: {
                return p_OpA + p_OpB;
            }

            // Branch
            case ALU_OP_TYPE::BRANCH: {
                // return PC + imm
                switch ((p_ALUFunct >> 1).to_ulong()) {

                    // BEQ
                    case 000:
                        if(p_OpA == p_OpB)          // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                        // Next instrution: return PC + 4
                            return p_OpA + 4;

                    // BNE
                    case 001:
                        if(p_OpA != p_OpB)          // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                        // Next instrution: return PC + 4
                            return p_OpA + 4;

                    // BLT
                    case 100:
                        if (p_OpA < p_OpB)          // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                        // Next instrution: return PC + 4
                            return p_OpA + 4;

                    // BGE
                    case 101:
                        if (p_OpA >= p_OpB)         // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                        // Next instrution: return PC + 4
                            return p_OpA + 4;

                    // BLTU
                    case 110:
                        if (static_cast<uint32_t>(p_OpA) < static_cast<uint32_t>(p_OpB)) // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                                                             // Next instrution: return PC + 4
                            return p_OpA + 4;

                    // BGEU
                    case 111:
                        if (static_cast<uint32_t>(p_OpA) >= static_cast<uint32_t>(p_OpB)) // Jump: return PC + imm
                            return p_OpA + p_OpB;
                        else                                                              // Next instrution: return PC + 4
                            return p_OpA + 4;

                    default:
                        std::string err_msg = std::format("ALU M-ext: Invalid funct3: {}", (p_ALUFunct >> 1).to_ulong());
                        throw RV32IM::ValueError (err_msg);
                        return 0;
                }
            }

            case ALU_OP_TYPE::R_TYPE: {
                switch (p_ALUFunct.to_ulong()) {
                    // ADD
                    case 0b0000:
                        return Adder(p_OpA, p_OpB, false, carryOut);

                    // SUB
                    case 0b0001:
                        complement = ~static_cast<uint32_t>(p_OpB);
                        return Adder(p_OpA, complement, true, carryOut);


                    // SLL
                    case 0b0010:
                        return p_OpA << (p_OpB & 0x1F);

                    // SLT
                    case 0b0100:
                        return (p_OpA < p_OpB) ? 1 : 0;

                     // SLTU
                    case 0b0110:
                        return (static_cast<uint32_t>(p_OpA) < static_cast<uint32_t>(p_OpB)) ? 1 : 0;

                    // XOR
                    case 0b1000:
                        return p_OpA ^ p_OpB;

                    // SRL
                    case 0b1010:
                        return static_cast<uint32_t>(p_OpA) >> (p_OpB & 0x1F);

                    // SRA
                    case 0b1011:
                        return p_OpA >> (p_OpB & 0x1F);

                    // OR
                    case 0b1100:
                        return p_OpA | p_OpB;

                    // AND
                    case 0b1110:
                        return p_OpA & p_OpB;

                    default:
                        std::string err_msg = std::format("ALU M-ext: Invalid funct3: {}", (p_ALUFunct >> 1).to_ulong());
                        throw RV32IM::ValueError (err_msg);
                        return 0;
                }
            }

            case ALU_OP_TYPE::I_TYPE: {
                switch (p_ALUFunct.to_ulong()) {

                    // ADDI
                    case 0b0000:
                    case 0b0001:
                        return Adder(p_OpA, p_OpB, false, carryOut);

                    // SLLI
                    case 0b0010:
                        return p_OpA << (p_OpB & 0x1F);

                    // SLTI
                    case 0b0100:
                    case 0b0101:
                        return (p_OpA < p_OpB) ? 1 : 0;

                    // SLTIU
                    case 0b0110:
                    case 0b0111:
                        return (static_cast<uint32_t>(p_OpA) < static_cast<uint32_t>(p_OpB)) ? 1 : 0;

                    // XORI
                    case 0b1001:
                    case 0b1000:
                        return p_OpA ^ p_OpB;

                     // SRLI
                    case 0b1010:
                        return static_cast<uint32_t>(p_OpA) >> (p_OpB & 0x1F);

                    // SRAI
                    case 0b1011:
                        return p_OpA >> (p_OpB & 0x1F);


                    // ORI
                    case 0b1100:
                    case 0b1101:
                        return p_OpA | p_OpB;

                    // ANDI
                    case 0b1110:
                    case 0b1111:
                        return p_OpA & p_OpB;

                    default:
                        std::string err_msg = std::format("ALU M-ext: Invalid funct3: {}", (p_ALUFunct >> 1).to_ulong());
                        throw RV32IM::ValueError (err_msg);
                        return 0;
                }
            }

            case ALU_OP_TYPE::M_Extension: {

                switch ((p_ALUFunct >> 1).to_ulong()) {
                    // MUL
                    case 0b000:
                        return MUL(p_OpA, p_OpB);

                    // MULH
                    case 0b001:
                        return MULH(p_OpA, p_OpB);


                    // MULSHU
                    case 0b010:
                        return MULHSU(p_OpA, p_OpB);

                    // MULHU
                    case 0b011:
                        return MULHU(p_OpA, p_OpB);

                    case 0b100:
                        return DIV(p_OpA, p_OpB);

                    case 0b101:
                        return DIVU(p_OpA, p_OpB);

                    case 0b110:
                        return REM(p_OpA, p_OpB);

                    case 0b111:
                        return REMU(p_OpA, p_OpB);

                    default:
                        std::string err_msg = std::format("ALU M-ext: Invalid funct3: {}", (p_ALUFunct >> 1).to_ulong());
                        throw RV32IM::ValueError (err_msg);
                        return 0;
                }
            }

            default:
                return 0;
        }
    }

}