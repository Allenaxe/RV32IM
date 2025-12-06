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

    // Determine calculate type
    // @Return: bitset<4> {funct3 (3 bit) + funct7 (1 bit)}
    std::bitset<4> ALU::AluControl (uint32_t p_funct3, uint32_t p_funct7) {
        std::bitset<4> type = p_funct3;
        type <<= 1;
        type[0] = (p_funct7 >> 5) & 1;
        return type;
    }

    int32_t ALU::Generate_OpA (const uint32_t PC, uint32_t p_Src1, bool p_Selector) {
        // signal: Branch / Jump = 1
        return p_Selector ? static_cast<int32_t>(PC): static_cast<int32_t>(p_Src1);
    }

    int32_t ALU::Generate_OpB (uint32_t p_Src2, const int32_t imm, bool p_Selector) {
        // signal: ALUsrc = 1
        return p_Selector ? static_cast<int32_t>(imm) : static_cast<int32_t>(p_Src2);
    }

    int32_t ALU::Operate (std::bitset<4> p_ALUFunct,
                          ALU_OP_TYPE p_ALUOp,
                          int32_t p_OpA,
                          int32_t p_OpB)
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
                        std::cerr << "Invaild [funct3]: " << (p_ALUFunct >> 1) << " !" << std::endl;
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
                        std::cerr << "Invaild [funct3]: " << (p_ALUFunct >> 1) << " !" << std::endl;
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
                        std::cerr << "Invaild [funct3]: " << (p_ALUFunct >> 1) << " !" << std::endl;
                        return 0;
                }
            }

            default:
                return 0;
        }
    }

}