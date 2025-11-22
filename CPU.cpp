#include "CPU.h"

namespace RV32IM {
    CPU::CPU(std::shared_ptr<Segmentation> p_ProgSeg): RF(new RegisterFile()), ProgSeg(p_ProgSeg) {}
    
    void CPU::Fetch() {
        MAR = PC;
        MDR = ProgSeg->Read(MDR);
        IR = MDR;
        PC += 1;
    }

    DecodeOutput CPU::Decode(uint32_t wd) { 
        // Instruction instr {};
        // instr.opcode = IR & 0x0000'007F;            // IR[6:0]
        // instr.rd = (IR & 0x0000'0F80) >> 7;         // IR[11:7]
        // instr.funct3 = (IR & 0x0000'7000) >> 12;    // IR[14:12]
        // instr.rs1 = (IR & 0x000F'8000) >> 15;       // IR[19:15]
        // instr.rs2 = (IR & 0x01F0'0000) >> 20;       // IR[24:20]
        // instr.funct7 = (IR & 0xFE00'0000) >> 25;    // IR[31:25]

        int32_t imm = ImmediateGenerator::Generate(IR);
        std::bitset<7> opcode {IR & 0x0000'007F};
        std::bitset<10> control_signal = ControlUnit::ControlSignal(opcode);
        uint8_t rs1 = (IR & 0x000F'8000) >> 15;
        uint8_t rs2 = (IR & 0x01F0'0000) >> 20;
        uint8_t rd = (IR & 0x0000'0F80) >> 7;
        std::bitset <3> funct3 = (IR & 0x0000'7000) >> 12;
        std::bitset <1> funct7 = (IR & 0x4000'0000) >> 30;

        RF->Write(rd, wd, control_signal[9]);
        RegisterFileRead RF_read = RF->Read(rs1, rs2);

        return DecodeOutput {imm, RF_read.rs1, RF_read.rs2, funct3, funct7, control_signal};
    }

    // void CPU::Print(Instruction &instr, uint32_t imm) {
    //     std::cout << "Original insturction:\n";
    //     std::cout << std::bitset<32>(IR) << std::endl << std::endl;

    //     std::cout << "Instruction fields in binary:\n";
    //     std::cout << "------------------------------------------------\n";
    //     std::cout << "opcode : " << instr.opcode << '\n';
    //     std::cout << "rd     : " << instr.rd << '\n';
    //     std::cout << "funct3 : " << instr.funct3 << '\n';
    //     std::cout << "rs1    : " << instr.rs1 << '\n';
    //     std::cout << "rs2    : " << instr.rs2 << '\n';
    //     std::cout << "funct7 : " << instr.funct7 << '\n';
    //     std::cout << "imm    : " << std::bitset<32>(imm) << '\n';
    //     std::cout << "------------------------------------------------\n";
    // }

    ExecuteOutput CPU::Execute() {
        Fetch();
        uint32_t wd = 0;                // write back data
        DecodeOutput decode_output = Decode(wd);

        int32_t opA = ALU::OpA(PC, decode_output.rs1, (decode_output.control_signal[4] | decode_output.control_signal[5]));
        int32_t opB = ALU::OpB(decode_output.rs2, decode_output.imm, decode_output.control_signal[8]);
        uint32_t aluControl = ALU::AluControl(decode_output.funct3.to_ulong(), decode_output.funct7.to_ulong());
        int32_t control_signal = decode_output.control_signal.to_ulong() & 0b111;
        int32_t alu_output = ALU::Operate(aluControl, control_signal, opA, opB);


        std::cout << "Immediate: " << decode_output.imm << '\n';
        std::cout << "rs1: " << decode_output.rs1 << '\n';
        std::cout << "rs2: " << decode_output.rs2 << '\n';
        std::cout << "Control Signal: " << decode_output.control_signal << '\n';
        std::cout << "ALU Output: " << alu_output << std::endl;
        
        return ExecuteOutput {static_cast<uint32_t>(alu_output), alu_output == 0};
    }
}