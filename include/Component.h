#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstdint>
#include <bitset>
#include <tuple>
#include <memory>

#include "Structure.h"
#include "Memory.h"

#define NUMBER_REGISTER 32

namespace RV32IM {

    class ControlUnit {
        /********************************************************
         * - MemSize (2 bits) - Data width for DataMemory load/store
         *     - `00` : byte (8 bits)
         *     - `01` : half word (16 bits)
         *     - `10` : full word (32 bits)
         *     - `11` : reserved
         * - RegWrite ( 1 bit ) : register can write
         * - ALUSrc ( 1 bit ) : `Op2` in ALU is `immediate` or `rs2`
         * - MemRW ( 2 bit ) : enable data memory to read/write
         *     - 00: Skip MEM stage
         *     - 01: Able to read
         *     - 10: Able to write
         *     - 11: Error
         * - Branch ( 1 bit ) : branch instruction
         * - Jump ( 1 bit ) : Jump instruction
         * - MemtoReg ( 1 bit ) - data memory write into register
         * - ALUOp ( 3 bits ) - ALU Operation
         *     - `000` : Load / Store
         *     - `001` : Branch
         *     - `010` : R Type
         *     - `011` : I Type
         *     - Others: Reserved
        ********************************************************/

        public:
            static ControlSignal Generate(std::bitset<7> &p_Opcode, std::bitset<3> &funct3);
    };

    class RegisterFile {
        private:
            uint32_t Register[NUMBER_REGISTER];
        public:
            RegisterFileRead Read(uint8_t rs1, uint8_t rs2);
            void Write(uint8_t rd, uint32_t wd, bool we);
    };

    class ImmediateGenerator {
        public:
            static uint32_t Generate(uint32_t &p_Instr);
        private:
            static uint32_t DecodeType(std::bitset<7> p_Opcode);
            static uint32_t Extend(uint32_t p_Immediate, uint32_t p_Extend);
    };

}

#endif