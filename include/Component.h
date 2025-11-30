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

	class ALU {
        public:
			/********************************************************
			 * - AluControl ( 4 bits ) - 3 bits funct3 & 1 bit funct7
			********************************************************/
			static int32_t Operate(std::bitset<4> p_aluOp, ALU_OP_TYPE control_signal, int32_t p_opA, int32_t p_opB);
			static int32_t OpA(uint32_t PC, uint32_t src1, bool p_Selector);
			static int32_t OpB(uint32_t src2, int32_t imm, bool p_Selector);
			static std::bitset<4> AluControl (uint32_t p_funct3, uint32_t p_funct7);

		private:
            static int32_t Adder(uint32_t src1, uint32_t src2, bool cin, bool &carry);
    };
}

#endif