#ifndef CPU_COMPONENT
#define CPU_COMPONENT
#include <utility>
#include <cstdint>

#define NUMBER_REGISTER 32

namespace RV32IM {
	class RegisterFile {
		private:
			uint32_t Register[NUMBER_REGISTER];
		public:
			auto read(uint8_t rs1, uint8_t rs2);
			void write(uint8_t rd, uint32_t wd, bool we);
	};

    class SignExtend {
        public:
            static int32_t extend(uint32_t p_Immediate, uint32_t p_extend);
    };
}

#endif