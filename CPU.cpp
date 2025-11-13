#include "CPU.h"

namespace RV32IM {
    CPU::CPU(Memory* p_TheMemory): TheMemory(p_TheMemory) {}
    
    void CPU::Fetch() {
        MAR = PC;
        MDR = TheMemory->Read(MDR);
        IR = MDR;
        PC += 1;
    }

    int32_t CPU::DecodeImm(std::bitset<7> p_Opcode) {
        uint8_t uint8_opcode = static_cast<uint8_t>(p_Opcode.to_ulong());
        switch(uint8_opcode) {
            case 0x13: // ALU imm
                return SignExtend::extend (IR >> 20, 12);

            case 0x03: // LOAD
                return SignExtend::extend (IR >> 20, 12);

            case 0x23: // STORE
                return SignExtend::extend (((IR >> 25) << 5) | ((IR >> 7) & 0x1F), 12);

            case 0x63: // BRANCH
                return SignExtend::extend (((IR >> 31) << 12) | (((IR >> 7) & 0x01) << 11)
                    | (((IR >> 25) & 0x3F) << 5)
                    | (((IR >> 8) & 0x0F) << 1),
                    11);

            case 0x37: // LUI
                return (IR & ((1 << 20) - 1));

            case 0x17: // AUIPC
                return (IR & ((1 << 20) - 1));

            case 0x6F: // JAL
                return SignExtend::extend (((IR >> 31) << 20) | (((IR >> 12) & 0xFF) << 12)
                    | (((IR >> 20) & 0x01) << 11)
                    | (((IR >> 25) & 0x3F) << 5)
                    | (((IR >> 21) & 0x0F) << 1),
                    12);

            default:
                return 0;
        }
    }

    Instruction CPU::Decode() { 
        Instruction instr {};
        instr.opcode = IR & 0x0000'007F;            // IR[6:0]
        instr.rd = (IR & 0x0000'0F80) >> 7;         // IR[11:7]
        instr.funct3 = (IR & 0x0000'7000) >> 12;    // IR[14:12]
        instr.rs1 = (IR & 0x000F'8000) >> 15;       // IR[19:15]
        instr.rs2 = (IR & 0x01F0'0000) >> 20;       // IR[24:20]
        instr.funct7 = (IR & 0xFE00'0000) >> 25;    // IR[31:25]
        instr.imm = DecodeImm(instr.opcode);
        return instr;
    }

    void CPU::Print(Instruction &instr) {
        std::cout << "Original insturction:\n";
        std::cout << std::bitset<32>(IR) << std::endl << std::endl;

        std::cout << "Instruction fields in binary:\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "opcode : " << instr.opcode << '\n';
        std::cout << "rd     : " << instr.rd << '\n';
        std::cout << "funct3 : " << instr.funct3 << '\n';
        std::cout << "rs1    : " << instr.rs1 << '\n';
        std::cout << "rs2    : " << instr.rs2 << '\n';
        std::cout << "funct7 : " << instr.funct7 << '\n';
        std::cout << "imm    : " << std::bitset<32>(instr.imm) << '\n';
        std::cout << "------------------------------------------------\n";
    }

    void CPU::Execute() {
        Fetch();
        Instruction instr = Decode();
        Print(instr);
    }
}