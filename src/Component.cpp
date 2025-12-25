#include "Component.h"
#include "Exception.h"
#include "CPU.h"

namespace RV32IM {

    RegisterFileRead RegisterFile::Read (uint8_t rs1, uint8_t rs2) {
        return RegisterFileRead {Register[rs1], Register[rs2]};
    }

    void RegisterFile::Write (uint8_t rd, uint32_t p_WriteData, bool p_WriteEnable) {
        if (rd == 0) {
            throw ValueError("RegisterFile: `rd` cannot be zero!");
        }

        if (p_WriteEnable == 1)
            Register[rd] = p_WriteData;
    }

    ControlSignal ControlUnit::Generate (std::bitset<7> &p_Opcode, std::bitset<3> &p_funct3, std::bitset<7> &p_funct7) {
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0110011
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0010011
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0100011
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b1100011

        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];   // if p_Opcode == 0b0000011
        bool isJAL = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];                // if p_Opcode == 0b1101111
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];            // if p_Opcode == 0b1100111
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];             // if p_Opcode == 0b0110111
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0010111
        bool isECALL = p_Opcode[6] & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];           // if p_Opcode == 0b1110011

        bool RegWrite = isRType | isIType | isLOAD | isLUI | isAUIPC | isJAL;

        bool ALUSrc = isIType | isLOAD | isSType | isLUI | isAUIPC;

        MEM_RW MemRW =  static_cast<MEM_RW>((isSType << 1) & isLOAD);

        bool Branch = isBType | isJAL | isJALR;     // stall two cycles
        bool Jump = isJAL;                          // stall one cycle
        bool MemtoReg = isLOAD;
        bool Halt = isECALL;

        ALU_OP_TYPE ALUOp =
            (isRType & p_funct7[0])    ? ALU_OP_TYPE::M_Extension :
            (isRType & (~p_funct7[0])) ? ALU_OP_TYPE::R_TYPE :
            isIType                    ? ALU_OP_TYPE::I_TYPE :
            Branch                     ? ALU_OP_TYPE::BRANCH :
            isLUI                      ? ALU_OP_TYPE::LUI :
            isAUIPC                    ? ALU_OP_TYPE::AUIPC :
                                         ALU_OP_TYPE::MEMORY_REF;

        MEM_SIZE MemSize = static_cast<MEM_SIZE>(p_funct3.to_ulong() & 0b11);

        bool SignExt = ~(p_funct3[2]);

        return ControlSignal {
            ExecuteSignal { ALUSrc, Branch, Jump, isAUIPC, isLUI, ALUOp },
            MemorySignal { MemRW, SignExt, MemSize },
            WriteBackSignal { RegWrite, MemtoReg, Halt }
        };
    }

    uint32_t ImmediateGenerator::DecodeType (std::bitset<7> p_Opcode) {
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0110011
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0010011
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0100011
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b1100011


        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];   // if p_Opcode == 0b0000011
        bool isJAL = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];                // if p_Opcode == 0b1101111
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];            // if p_Opcode == 0b1100111
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];             // if p_Opcode == 0b0110111
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0010111

        return
            (isRType    << 8) |
            (isIType    << 7) |
            (isSType    << 6) |
            (isBType    << 5) |
            (isJAL      << 4) |
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
                return Extend (((p_Instr >> 31) << 12)          |
                               (((p_Instr >> 7)  & 0x01) << 11) |
                               (((p_Instr >> 25) & 0x3F) << 5)  |
                               (((p_Instr >> 8)  & 0x0F) << 1)
                               , 11
                );

            case (1 << 4):          // J Type
                return Extend (((p_Instr >> 31) << 20) |
                               (((p_Instr >> 12) & 0xFF) << 12) |
                               (((p_Instr >> 20) & 0x01) << 11) |
                               (((p_Instr >> 25) & 0x3F) << 5)  |
                               (((p_Instr >> 21) & 0x0F) << 1)
                               , 12
                );

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

}