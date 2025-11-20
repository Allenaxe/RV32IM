#include "CPU.h"

namespace RV32IM {
    CPU::CPU(Memory* p_TheMemory): RF(new RegisterFile()), TheMemory(p_TheMemory) {}
    void CPU::Fetch() {
        MAR = PC;
        MDR = TheMemory->Read(MDR);
        IR = MDR;
        PC += 1;
    }

    DecodeOutput CPU::Decode() { 
        std::bitset<7> opcode {IR & 0x0000'007F};            // IR[6:0]
        std::bitset<5> rd {(IR & 0x0000'0F80) >> 7};         // IR[11:7]
        std::bitset<3> funct3 {(IR & 0x0000'7000) >> 12};    // IR[14:12]
        uint8_t rs1 = (IR & 0x000F'8000) >> 15;              // IR[19:15]
        uint8_t rs2 = (IR & 0x01F0'0000) >> 20;              // IR[24:20]
        std::bitset<7> funct7 {(IR & 0xFE00'0000) >> 25};    // IR[31:25]

        int32_t imm = ImmediateGenerator::Generate(IR);
        std::bitset<10> control_signal = ControlUnit::ControlSignal(opcode);

        RegisterFileRead RF_read = RF->Read(rs1, rs2);

        return DecodeOutput {imm, RF_read.rs1, RF_read.rs2, rd, funct3, funct7, control_signal};
    }

    void CPU::Print() {
        for (auto& out : Trace) {
            std::visit(Printer(), out);
        }
    }

    void CPU::Execute() {
        Fetch();
        // uint32_t wd = 0;                // write back data
        DecodeOutput decode_output = Decode();
        Trace.push_back(decode_output);
    }
}