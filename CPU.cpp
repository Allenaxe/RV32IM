#include "CPU.h"

namespace RV32IM {
  CPU::CPU(Memory* p_TheMemory): TheMemory(p_TheMemory) {}
  void CPU::Fetch() {
    MAR = ProgramCounter;
    MDR = TheMemory->Read(MAR);
    IR = MDR;
    ProgramCounter += 1;
  }
  int32_t CPU::SignExtend(uint32_t p_Immediate, uint32_t p_extend) {
    int32_t mask = 1 << (p_extend - 1);
    return (p_Immediate ^ mask) - mask;
  }
  int32_t CPU::DecodeImm(uint8_t p_Opcode) {
    switch(p_Opcode) {
      case 0x13: return SignExtend(IR >> 20, 12); // ALU imm
      case 0x03: return SignExtend(IR >> 20, 12); // LOAD
      case 0x23: return SignExtend(((IR >> 25) << 5) | ((IR >> 7) & 0x1F), 12); // STORE
      case 0x63: return SignExtend(((IR >> 31) << 12) | (((IR >> 7) & 0x01) << 11) | (((IR >> 25) & 0x3F) << 5) | (((IR >> 8) & 0x0F) << 1), 11); // BRANCH
      case 0x37: return (IR & ((1 << 20) - 1)); // LUI
      case 0x17: return (IR & ((1 << 20) - 1)); // AUIPC
      case 0x6F: return SignExtend(((IR >> 31) << 20) | (((IR >> 12) & 0xFF) << 12) | (((IR >> 20) & 0x01) << 11) | (((IR >> 25) & 0x3F) << 5) | (((IR >> 21) & 0x0F) << 1), 12); // JAL
      default: return 0;
    }
  }
  Instruction CPU::Decode() {
    Instruction ins {};
    ins.opcode = IR & 0x7F;
    ins.rd = (IR >> 7) & 0x1F;
    ins.funct3 = (IR >> 12) & 0x07;
    ins.rs1 = (IR >> 15) & 0x1F;
    ins.rs2 = (IR >> 20) & 0x1F;
    ins.funct7 = (IR >> 25) & 0x7F;
    ins.imm = DecodeImm(ins.opcode);
    return ins;
  }
  void CPU::Print(Instruction &ins) {
    std::cout << "Instruction fields in binary:\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "opcode : " << std::bitset<7>(ins.opcode) << '\n';
    std::cout << "rd     : " << std::bitset<5>(ins.rd) << '\n';
    std::cout << "funct3 : " << std::bitset<3>(ins.funct3) << '\n';
    std::cout << "rs1    : " << std::bitset<5>(ins.rs1) << '\n';
    std::cout << "rs2    : " << std::bitset<5>(ins.rs2) << '\n';
    std::cout << "funct7 : " << std::bitset<7>(ins.funct7) << '\n';
    std::cout << "imm    : " << std::bitset<32>(ins.imm) << '\n';
    std::cout << "------------------------------------------------\n";
  }
  void CPU::Execute() {
    Fetch();
    Instruction ins = Decode();
    Print(ins);
  }
}