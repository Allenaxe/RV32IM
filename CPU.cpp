#include "CPU.h"

namespace RV32IM {
  CPU::CPU(Memory* p_TheMemory): TheMemory(p_TheMemory) {}
  void CPU::Fetch() {
    MemoryAddressRegister = ProgramCounter;
    MemoryDataRegister = TheMemory->Read(MemoryDataRegister);
    InstructionRegister = MemoryDataRegister;
    ProgramCounter += 1;
  }
  int32_t CPU::DecodeImm(uint8_t p_Opcode) {
    switch(p_Opcode) {
      case 0x13: return SignExtend::extend(InstructionRegister >> 20, 12); // ALU imm
      case 0x03: return SignExtend::extend(InstructionRegister >> 20, 12); // LOAD
      case 0x23: return SignExtend::extend(((InstructionRegister >> 25) << 5) | ((InstructionRegister >> 7) & 0x1F), 12); // STORE
      case 0x63: return SignExtend::extend(((InstructionRegister >> 31) << 12) | (((InstructionRegister >> 7) & 0x01) << 11) | (((InstructionRegister >> 25) & 0x3F) << 5) | (((InstructionRegister >> 8) & 0x0F) << 1), 11); // BRANCH
      case 0x37: return (InstructionRegister & ((1 << 20) - 1)); // LUI
      case 0x17: return (InstructionRegister & ((1 << 20) - 1)); // AUIPC
      case 0x6F: return SignExtend::extend(((InstructionRegister >> 31) << 20) | (((InstructionRegister >> 12) & 0xFF) << 12) | (((InstructionRegister >> 20) & 0x01) << 11) | (((InstructionRegister >> 25) & 0x3F) << 5) | (((InstructionRegister >> 21) & 0x0F) << 1), 12); // JAL
      default: return 0;
    }
  }
  Instruction CPU::Decode() {
    Instruction instr {};
    instr.opcode = InstructionRegister & 0x7F;
    instr.rd = (InstructionRegister >> 7) & 0x1F;
    instr.funct3 = (InstructionRegister >> 12) & 0x07;
    instr.rs1 = (InstructionRegister >> 15) & 0x1F;
    instr.rs2 = (InstructionRegister >> 20) & 0x1F;
    instr.funct7 = (InstructionRegister >> 25) & 0x7F;
    instr.imm = DecodeImm(instr.opcode);
    return instr;
  }
  void CPU::Print(Instruction &instr) {
    std::cout << "Instruction fields in binary:\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "opcode : " << std::bitset<7>(instr.opcode) << '\n';
    std::cout << "rd     : " << std::bitset<5>(instr.rd) << '\n';
    std::cout << "funct3 : " << std::bitset<3>(instr.funct3) << '\n';
    std::cout << "rs1    : " << std::bitset<5>(instr.rs1) << '\n';
    std::cout << "rs2    : " << std::bitset<5>(instr.rs2) << '\n';
    std::cout << "funct7 : " << std::bitset<7>(instr.funct7) << '\n';
    std::cout << "imm    : " << std::bitset<32>(instr.imm) << '\n';
    std::cout << "------------------------------------------------\n";
  }
  void CPU::Execute() {
    Fetch();
    Instruction instr = Decode();
    Print(instr);
  }
}