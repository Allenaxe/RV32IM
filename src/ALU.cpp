#include "ALU.h"

namespace RV32IM {

    int32_t ALU::Adder (uint32_t src1, uint32_t src2, bool cin, bool &carry) {
        uint32_t result = 0;
        bool carry_local = cin;

        for(int i = 0; i < 32; i++){
            bool bit1 = (src1 >> i) & 1;
            bool bit2 = (src2 >> i) & 1;
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

    std::bitset<4> ALU::AluControl (uint32_t p_funct3, uint32_t p_funct7){
        // return funct3(3bit)+funct7(1bit)
        // determine calculate type
        std::bitset<4> type = p_funct3;
        type <<= 1;
        type[0] = (p_funct7 >> 5) & 1;
        return type;
    }

    int32_t ALU::OpA (uint32_t PC, uint32_t src1, bool p_Selector) {
        // signal: Branch / Jump = 1
        return p_Selector ? static_cast<int32_t>(PC): static_cast<int32_t>(src1);
    }

    int32_t ALU::OpB (uint32_t src2, int32_t imm, bool p_Selector) {
        // signal: ALUsrc = 1
        return p_Selector ? static_cast<int32_t>(imm) : static_cast<int32_t>(src2);
    }

    // int32_t ALU::Operate(DecodeOutput p_DecodeOutput)
    int32_t ALU::Operate (std::bitset<4> p_aluOp, ALU_OP_TYPE control_signal, int32_t p_opA, int32_t p_opB) {

        bool carryOut = false; // Carry Flag
        uint32_t complement;

        switch (control_signal) {
            // Load / Store
            case ALU_OP_TYPE::MEMORY_REF: {
                return p_opA + p_opB;
            }

            // Branch
            case ALU_OP_TYPE::BRANCH: {
                // return PC + imm
                switch ((p_aluOp >> 1).to_ulong()) {

                    // BEQ
                    case 000:
                        if(p_opA == p_opB)          // return PC + imm
                            return p_opA + p_opB;
                        else                        // return PC + 4
                            return p_opA + 4;

                    // BNE
                    case 001:
                        if(p_opA != p_opB)          // return PC + imm
                            return p_opA + p_opB;
                        else                        // return PC + 4
                            return p_opA + 4;

                    // BLT
                    case 100:
                        if(static_cast<int32_t>(p_opA) < static_cast<int32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                          // return PC + 4
                            return p_opA + 4;

                    // BGE
                    case 101:
                        if(static_cast<int32_t>(p_opA) >= static_cast<int32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                           // return PC + 4
                            return p_opA + 4;

                    // BLTU
                    case 110:
                        if(static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                            // return PC + 4
                            return p_opA + 4;

                    // BGEU
                    case 111:
                        if(static_cast<uint32_t>(p_opA) >= static_cast<uint32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                             // return PC + 4
                            return p_opA + 4;

                    default:
                        std::cerr << "Invaild [funct3]: " << (p_aluOp >> 1) << " !" << std::endl;
                        return 0;
                }
            }

            case ALU_OP_TYPE::R_TYPE: { // R-type
                switch (p_aluOp.to_ulong()) {
                    // ADD
                    case 0b0000:
                        return Adder(p_opA, p_opB, false, carryOut);

                    // SUB
                    case 0b0001:
                        complement = ~static_cast<uint32_t>(p_opB);
                        return Adder(p_opA, complement, true, carryOut);


                    // SLL
                    case 0b0010:
                        return p_opA << (p_opB & 0x1F);

                    // SLT
                    case 0b0100:
                        return (p_opA < p_opB) ? 1 : 0;

                     // SLTU
                    case 0b0110:
                        return (static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) ? 1 : 0;

                    // XOR
                    case 0b1000:
                        return p_opA ^ p_opB;

                    // SRL
                    case 0b1010:
                        return static_cast<uint32_t>(p_opA) >> (p_opB & 0x1F);

                    // SRA
                    case 0b1011:
                        return p_opA >> (p_opB & 0x1F);

                    // OR
                    case 0b1100:
                        return p_opA | p_opB;

                    // AND
                    case 0b1110:
                        return p_opA & p_opB;

                    default:
                        std::cerr << "Invaild [funct3]: " << (p_aluOp >> 1) << " !" << std::endl;
                        return 0;
                }
            }

            case ALU_OP_TYPE::I_TYPE: {       // I-type
                switch (p_aluOp.to_ulong()) {

                    // ADDI
                    case 0b0000:
                    case 0b0001:
                        return Adder(p_opA, p_opB, false, carryOut);

                    // SLLI
                    case 0b0010:
                        return p_opA << (p_opB & 0x1F);

                    // SLTI
                    case 0b0100:
                    case 0b0101:
                        return (p_opA < p_opB) ? 1 : 0;

                    // SLTIU
                    case 0b0110:
                    case 0b0111:
                        return (static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) ? 1 : 0;

                    // XORI
                    case 0b1001:
                    case 0b1000:
                        return p_opA ^ p_opB;

                     // SRLI
                    case 0b1010:
                        return static_cast<uint32_t>(p_opA) >> (p_opB & 0x1F);

                    // SRAI
                    case 0b1011:
                        return p_opA >> (p_opB & 0x1F);


                    // ORI
                    case 0b1100:
                    case 0b1101:
                        return p_opA | p_opB;

                    // ANDI
                    case 0b1110:
                    case 0b1111:
                        return p_opA & p_opB;

                    default:
                        std::cerr << "Invaild [funct3]: " << (p_aluOp >> 1) << " !" << std::endl;
                        return 0;
                }
            }

            default:
                return 0;
        }
    }

}