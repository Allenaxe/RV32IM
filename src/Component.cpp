#include "Component.h"

namespace RV32IM {
    RegisterFileRead RegisterFile::Read (uint8_t rs1, uint8_t rs2) {
        return RegisterFileRead {Register[rs1], Register[rs2]};
    }

    void RegisterFile::Write (uint8_t rd, uint32_t wd, bool we) {
        if (rd != 0 && we == 1)
            Register[rd] = wd;
    }

    ControlSignal ControlUnit::Generate (std::bitset<7> &p_Opcode, std::bitset<3> &p_funct3) {
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isJType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];

        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];

        bool RegWrite = isRType | isIType | isLOAD | isLUI | isAUIPC | isJType;
        bool ALUSrc = isIType | isLOAD | isSType;
        bool MemRead = isLOAD;
        bool MemWrite = isSType;
        bool Branch = isBType;
        bool Jump = isJType | isJALR;
        bool MemtoReg = isLOAD;

        ALU_OP_TYPE ALUOp = static_cast<ALU_OP_TYPE>(((isRType | isIType) << 1) | (Branch | isIType));

        MEM_SIZE MemSize = static_cast<MEM_SIZE>(p_funct3.to_ulong() & 0b11);

        bool Signext = !p_funct3[2];

        return ControlSignal {
            ExecuteSignal { ALUSrc, Branch, Jump, ALUOp },
            MemorySignal { MemRead, MemWrite, Signext, MemSize },
            WriteBackSignal { RegWrite, MemtoReg }
        };
    }

    std::bitset<13> ControlUnit::SerializeControlSignal(const ControlSignal& control) {
        std::bitset<13> output;
        size_t current_bit = 0; // 從 LSB (0) 開始計數

        // --- 寫回階段 (WB) ---
        // RegWrite [0]
        output[current_bit++] = control.wb_signal.RegWrite;
        
        // MemToReg [1]
        output[current_bit++] = control.wb_signal.MemToReg;

        // --- 記憶體階段 (MEM) ---
        // MemRead [2]
        output[current_bit++] = control.mem_signal.MemRead;
        
        // MemWrite [3]
        output[current_bit++] = control.mem_signal.MemWrite;
        
        // Signext [4]
        output[current_bit++] = control.mem_signal.Signext;

        // MemSize [6:5] - 2 bits (使用 to_ulong 取得數值)
        // 這裡需要將 Enum 數值按位元寫入
        uint8_t mem_size_val = static_cast<uint8_t>(control.mem_signal.MemSize);
        for (int i = 0; i < 2; ++i) {
            output[current_bit++] = (mem_size_val >> i) & 1;
        }

        // --- 執行階段 (EX) ---
        // ALUSrc [7]
        output[current_bit++] = control.ex_signal.ALUSrc;
        
        // Branch [8]
        output[current_bit++] = control.ex_signal.Branch;
        
        // Jump [9]
        output[current_bit++] = control.ex_signal.Jump;

        // ALUOp [12:10] - 3 bits (使用 to_ulong 取得數值)
        uint8_t alu_op_val = static_cast<uint8_t>(control.ex_signal.ALUOp);
        for (int i = 0; i < 3; ++i) {
            output[current_bit++] = (alu_op_val >> i) & 1;
        }

        return output;
    }

    uint32_t ImmediateGenerator::DecodeType (std::bitset<7> p_Opcode) {
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isJType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];

        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];

        return
            (isRType    << 8) |
            (isIType    << 7) |
            (isSType    << 6) |
            (isBType    << 5) |
            (isJType    << 4) |
            (isLOAD     << 3) |
            (isJALR     << 2) |
            (isLUI      << 1) |
            (isAUIPC        ) ;
    };

    uint32_t ImmediateGenerator::Extend (uint32_t p_Immediate, uint32_t p_Extend) {
        int32_t mask = 1 << (p_Extend - 1);
        return (p_Immediate ^ mask) - mask;
    };

    uint32_t ImmediateGenerator::Generate (uint32_t &p_Instr) {

        std::bitset<7> opcode {p_Instr & 0x0000'007F};

        uint32_t InstrctionType = DecodeType(opcode);

        switch(InstrctionType) {
            case (1 << 7):          // I Type
                return Extend (p_Instr >> 20, 12);

            case (1 << 6):          // S Type
                return Extend (((p_Instr >> 25) << 5) | ((p_Instr >> 7) & 0x1F), 12);

            case (1 << 5):          // B Type
                return Extend (((p_Instr >> 31) << 12) | (((p_Instr >> 7) & 0x01) << 11)
                    | (((p_Instr >> 25) & 0x3F) << 5)
                    | (((p_Instr >> 8) & 0x0F) << 1),
                    11);

            case (1 << 4):          // J Type
                return Extend (((p_Instr >> 31) << 20) | (((p_Instr >> 12) & 0xFF) << 12)
                    | (((p_Instr >> 20) & 0x01) << 11)
                    | (((p_Instr >> 25) & 0x3F) << 5)
                    | (((p_Instr >> 21) & 0x0F) << 1),
                    12);

            case (1 << 3):          // LOAD
                return Extend (p_Instr >> 20, 12);

            case (1 << 1):          // LUI
                return (p_Instr & (((1 << 20) - 1) << 12));

            case (1):               // AUIPC
                return (p_Instr & (((1 << 20) - 1) << 12));

            default:
                return 0;
        }
    }

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