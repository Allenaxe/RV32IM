#ifndef CPU_COMPONENT
#define CPU_COMPONENT
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
		 * - RegWrite ( 1 bit ) : register can write
		 * - ALUSrc ( 1 bit ) : `Op2` in ALU is `immediate` or `rs2`
		 * - MemRead ( 1 bit ) : data memory can read
		 * - MemWrite ( 1 bit ) : data memory can write
		 * - Branch ( 1 bit ) : branch instruction
		 * - Jump ( 1 bit ) : Jump instruction
		 * - MemtoReg ( 1 bit ) - data memory write into register
		 * - ALUOp ( 3 bits ) - ALU Operation
		 * 	- `000` : Load / Store
		 * 	- `001` : Branch
		 *  - `010` : R Type
		 * 	- `011` : I Type
		 * 	- Other: Reserved
		********************************************************/
		public:
			static std::bitset <10> ControlSignal(std::bitset<7> &p_Opcode);
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