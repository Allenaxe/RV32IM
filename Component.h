#ifndef CPU_COMPONENT
#define CPU_COMPONENT
#include <utility>
#include <cstdint>

#define NUMBER_REGISTER 32
#include <bitset>
#include "Instruction.h"

namespace RV32IM {
	class ControlUnit {
		public:
			std::bitset <9> control_signal(std::bitset<7> p_Opcode);
	};
	class RegisterFile {
		private:
			uint32_t Register[NUMBER_REGISTER];
		public:
			auto read(uint8_t rs1, uint8_t rs2);
			void write(uint8_t rd, uint32_t wd, bool we);
	};

    class ImmediateGenerator {
        public:
			static uint32_t generate(Instruction &instr);
		private:
			uint32_t decodetype(std::bitset<7> p_Opcode);
			uint32_t extend(uint32_t p_Immediate, uint32_t p_extend);
    };
}

#endif