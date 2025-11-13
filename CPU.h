#ifndef CPU_PROCESSOR
#define CPU_PROCESSOR

#include <iostream>
#include <cstdint>
#include <bitset>
#include "Memory.h"
#include "Component.h"
#include "Structure.h"

namespace RV32IM {
    class CPU {
        // public:
        //     const byte c_ReservedAddress;   const byte c_BaseAddress;   const byte c_AddressCeiling;

        private:
            uint32_t PC;                            // ProgramCounter
            uint32_t MAR;                           // MemoryAddressRegister
            uint32_t MDR;                           // MemoryDataRegister
            uint32_t IR;                            // InstructionRegister

            // bool m_OverflowError;
            // bool m_UnderflowError;
            // bool m_SignedMode;
            // bool m_Halt;

            RegisterFile* RF;

            Memory* TheMemory;
            void Fetch();
            DecodeOutput Decode(uint32_t wd);

            // void Print(Instruction &instr, uint32_t imm);

        public:
            CPU(Memory* p_TheMemory);
            // ~CPU();
            // void Reset();
            void Execute();
    };
}
#endif