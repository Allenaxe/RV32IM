#include "Component.h"

namespace RV32IM {
    RegisterFileRead RegisterFile::read(uint8_t rs1, uint8_t rs2) {
        return RegisterFileRead {Register[rs1], Register[rs2]};
    }

    void RegisterFile::write(uint8_t rd, uint32_t wd, bool we) {
        if (rd != 0 && we == 1)
            Register[rd] = wd;
    }

    std::bitset <10> ControlUnit::control_signal(std::bitset<7> &p_Opcode) {
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
        bool ALUOp_2 = false;
        bool ALUOp_1 = isRType | isIType;
        bool ALUOp_0 = Branch | isIType;

        unsigned long control_signal = 
            (RegWrite   << 9)  |
            (ALUSrc     << 8)  |
            (MemRead    << 7)  |
            (MemWrite   << 6)  |
            (Branch     << 5)  |
            (Jump       << 4)  |
            (MemtoReg   << 3)  |
            (ALUOp_2    << 2)  |
            (ALUOp_1    << 1)  |
            (ALUOp_0        )  ;

        return std::bitset <10> {control_signal};
    }

    uint32_t ImmediateGenerator::decodetype(std::bitset<7> p_Opcode) {
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

    int32_t ImmediateGenerator::extend(uint32_t p_Immediate, uint32_t p_Extend) {
        int32_t mask = 1 << (p_Extend - 1);
        return (p_Immediate ^ mask) - mask;
    };

    int32_t ImmediateGenerator::generate(uint32_t &p_Instr) {

        std::bitset<7> opcode {p_Instr & 0x0000'007F}; 

        uint32_t InstrctionType = decodetype(opcode);
        
        switch(InstrctionType) {
            case (1 << 7):          // I Type
                return extend (p_Instr >> 20, 12);

            case (1 << 6):          // S Type
                return extend (((p_Instr >> 25) << 5) | ((p_Instr >> 7) & 0x1F), 12);
            
            case (1 << 5):          // B Type
                return extend (((p_Instr >> 31) << 12) | (((p_Instr >> 7) & 0x01) << 11)
                    | (((p_Instr >> 25) & 0x3F) << 5)
                    | (((p_Instr >> 8) & 0x0F) << 1),
                    11);

            case (1 << 4):          // J Type
                return extend (((p_Instr >> 31) << 20) | (((p_Instr >> 12) & 0xFF) << 12)
                    | (((p_Instr >> 20) & 0x01) << 11)
                    | (((p_Instr >> 25) & 0x3F) << 5)
                    | (((p_Instr >> 21) & 0x0F) << 1),
                    12);
            
            case (1 << 3):          // LOAD
                return extend (p_Instr >> 20, 12);

            case (1 << 1):          // LUI
                return (p_Instr & (((1 << 20) - 1) << 12));

            case (1):               // AUIPC
                return (p_Instr & (((1 << 20) - 1) << 12));

            default:
                return 0;
        }
    }

    int32_t ALU::Adder(uint32_t src1, uint32_t src2, bool cin, bool &carry) {
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

    uint32_t ALU::AluControl(uint8_t p_funct3, int8_t p_funct7){
        // return funct3(3bit)+funct7(1bit)
        // determine calculate type
        std::bitset<4> type = p_funct3;
        type <<= 1;
        type[0] = p_funct7;
        return type.to_ulong();
    }

    int32_t ALU::OpA(uint32_t PC, uint32_t src1, bool p_Selector) {
        // signal: Branch / Jump = 1
        return p_Selector ? static_cast<int32_t>(PC): static_cast<int32_t>(src1);
    }

    int32_t ALU::OpB(uint32_t src2, int32_t imm, bool p_Selector) {
        // signal: ALUsrc = 1
        return p_Selector ? static_cast<int32_t>(imm) : static_cast<int32_t>(src2);
    }

    // int32_t ALU::Operate(DecodeOutput p_DecodeOutput)
    int32_t ALU::Operate(uint32_t p_aluOp, int32_t control_signal, int32_t p_opA, int32_t p_opB) {
        switch ((control_signal)){
            case 0b000: // Load / Store
                return p_opA + p_opB;

            case 0b001: // Branch
                // return PC + imm
                switch (p_aluOp){
                    case 0000:
                    case 0001: // beq
                        if(p_opA == p_opB) // return PC + imm
                            return p_opA + p_opB;
                        else               // return PC + 4
                            return p_opA + 4; 

                    case 0010:
                    case 0011: // bne
                        if(p_opA != p_opB) // return PC + imm
                            return p_opA + p_opB;
                        else               // return PC + 4
                            return p_opA + 4; 

                    case 1000:
                    case 1001: // blt
                        if(static_cast<int32_t>(p_opA) < static_cast<int32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                          // return PC + 4
                            return p_opA + 4; 

                    case 1010:
                    case 1011: // bge
                        if(static_cast<int32_t>(p_opA) >= static_cast<int32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                           // return PC + 4
                            return p_opA + 4; 
                    case 1100:
                    case 1101: // bltu
                        if(static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                            // return PC + 4
                            return p_opA + 4; 

                    case 1110:
                    case 1111: // bgeu
                        if(static_cast<uint32_t>(p_opA) >= static_cast<uint32_t>(p_opB)) // return PC + imm
                            return p_opA + p_opB;
                        else                                                             // return PC + 4
                            return p_opA + 4; 
                }
            case 0b010:{ // R-type
                switch (p_aluOp){
                    case 0b0000: // ADD
                        bool carryOut = false; // Carry Flag
                        return Adder(p_opA, p_opB, false, carryOut);

                    case 0b0001: // SUB
                        bool carryOut = false; // Carry Flag
                        uint32_t complement = ~static_cast<uint32_t>(p_opB);
                        return Adder(p_opA, complement, true, carryOut);
                        
                    case 0b0010: // SLL
                        return p_opA << (p_opB & 0x1F);
                
                    case 0b0100: // SLT
                        return (p_opA < p_opB) ? 1 : 0;

                    case 0b0110: // SLTU
                        return (static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) ? 1 : 0;
                    
                    case 0b1000: // XOR
                        return p_opA ^ p_opB;

                    case 0b1010:  // SRL
                        return static_cast<uint32_t>(p_opA) >> (p_opB & 0x1F);

                    case 0b1011: // SRA
                        return p_opA >> (p_opB & 0x1F);

                    case 0b1100: // OR
                        return p_opA | p_opB;

                    case 0b1110: // AND
                        return p_opA & p_opB;
                    
                    default:
                        std::cerr << "Invaild [funct3]: " << (p_aluOp >> 1) << " !" << std::endl;
                        return 0;
                }
            }
            case 0b011:{ // I-type
                switch (p_aluOp){
                    case 0b0001:
                    case 0b0000: // ADDI
                        bool carryOut = false; // Carry Flag
                        return Adder(p_opA, p_opB, false, carryOut);
                        
                    case 0b0010: // SLLI
                        return p_opA << (p_opB & 0x1F);
                    
                    case 0b0101:
                    case 0b0100: // SLTI
                        return (p_opA < p_opB) ? 1 : 0;
                    case 0b0111:
                    case 0b0110: // SLTIU
                        return (static_cast<uint32_t>(p_opA) < static_cast<uint32_t>(p_opB)) ? 1 : 0;
                    
                    case 0b1001:
                    case 0b1000: // XORI
                        return p_opA ^ p_opB;
                    case 0b1010:  // SRLI
                        return static_cast<uint32_t>(p_opA) >> (p_opB & 0x1F);

                    case 0b1011: // SRAI
                        return p_opA >> (p_opB & 0x1F);

                    case 0b1101:
                    case 0b1100: // ORI
                        return p_opA | p_opB;
                    case 0b1111:
                    case 0b1110: // ANDI
                        return p_opA & p_opB;
                    
                    default:
                        std::cerr << "Invaild [funct3]: " << (p_aluOp >> 1) << " !" << std::endl;
                        return 0;
                }
            }
        }
    }
}