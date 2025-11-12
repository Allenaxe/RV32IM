#ifndef CPU_COMPONENT
#define CPU_COMPONENT
#include <cstdint>
#include <bitset>
#include <tuple>

#include "Structure.h"

#define NUMBER_REGISTER 32

namespace RV32IM {
	class ControlUnit {
		public:
			static std::bitset <10> control_signal(std::bitset<7> &p_Opcode);
	};
	class RegisterFile {
		private:
			uint32_t Register[NUMBER_REGISTER];
		public:
			RegisterFileRead read(uint8_t rs1, uint8_t rs2);
			void write(uint8_t rd, uint32_t wd, bool we);
	};

    class ImmediateGenerator {
        public:
			static int32_t generate(uint32_t &p_Instr);
		private:
			static uint32_t decodetype(std::bitset<7> p_Opcode);
			static int32_t extend(uint32_t p_Immediate, uint32_t p_Extend);
    };
}

#endif