#include "Component.h"

namespace RV32IM {
    auto RegisterFile::read(uint8_t rs1, uint8_t rs2) {
        return std::make_pair(Register[rs1], Register[rs2]);
    }

    void RegisterFile::write(uint8_t rd, uint32_t wd, bool we) {
        if (rd != 0 && we == 1)
            Register[rd] = wd;
    }

    // std::bitset <9> ControlUnit::control_signal(std::bitset<7> p_Opcode) {
    //     bool isRType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
    //     bool isIType = (~p_Opcode[6]) & (~p_Opcode[5]) & p_Opcode[4] & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
    //     bool isSType = (~p_Opcode[6]) & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
    //     bool isBType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];
    //     bool isUType = (~p_Opcode[6]) & p_Opcode[5] & p_Opcode[4] & (~p_Opcode[3]) & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
    //     bool isJType = p_Opcode[6] & p_Opcode[5] & (~p_Opcode[4]) & p_Opcode[3] & p_Opcode[2] & p_Opcode[1] & p_Opcode[0];
    //     bool isLoad = (~p_Opcode[6]) & (~p_Opcode[5]) & (~p_Opcode[4]) & (~p_Opcode[3]) & (~p_Opcode[2]) & p_Opcode[1] & p_Opcode[0];

    //     bool RegWrite = isRType | isIType | isLoad | isUType | isJType;
    //     bool ALUSrc = isIType | isLoad | isSType | isUType | isJType;
    //     bool MemRead = isLoad;
    //     bool MemWrite = isSType;
    //     bool Branch = isBType;
    //     bool Jump = isJType;
    //     bool MemtoReg = isLoad;
    //     bool ALUOp_1 = isRType | isIType;
    //     bool ALUOp_2 = is
    // }
}