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
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0110011
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0010011
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0100011
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b1100011
        bool isJType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];              // if p_Opcode == 0b1101111

        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];   // if p_Opcode == 0b0000011
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];            // if p_Opcode == 0b1100111
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];             // if p_Opcode == 0b0110111
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0010111

        bool RegWrite = isRType | isIType | isLOAD | isLUI | isAUIPC | isJType;
        bool ALUSrc = isIType | isLOAD | isSType;

        MEM_RW MemRW =  static_cast<MEM_RW>((isLOAD << 1) & isSType);

        bool Branch = isBType;
        bool Jump = isJType | isJALR;
        bool MemtoReg = isLOAD;

        ALU_OP_TYPE ALUOp = static_cast<ALU_OP_TYPE>(((isRType | isIType) << 1) | (Branch | isIType));

        MEM_SIZE MemSize = static_cast<MEM_SIZE>(p_funct3.to_ulong() & 0b11);

        bool SignExt = ~(p_funct3[2]);

        return ControlSignal {
            ExecuteSignal { ALUSrc, Branch, Jump, ALUOp },
            MemorySignal { MemRW, SignExt, MemSize },
            WriteBackSignal { RegWrite, MemtoReg }
        };
    }

    uint32_t ImmediateGenerator::DecodeType (std::bitset<7> p_Opcode) {
        bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0110011
        bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0010011
        bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];     // if p_Opcode == 0b0100011
        bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b1100011
        bool isJType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];              // if p_Opcode == 0b1101111

        bool isLOAD = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];   // if p_Opcode == 0b0000011
        bool isJALR = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];            // if p_Opcode == 0b1100111
        bool isLUI = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];             // if p_Opcode == 0b0110111
        bool isAUIPC = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];        // if p_Opcode == 0b0010111

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