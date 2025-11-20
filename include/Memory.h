
#ifndef CPU_MEMORY
#define CPU_MEMORY
#include <stdint.h>

namespace RV32IM {
    typedef unsigned char byte;

    class MainMemory {
        public:
            const uint32_t c_MaxAddress;

        private:
            uint32_t* m_MemorySpace;

        public:
            MainMemory();
            ~MainMemory();
            void Clear();
            uint32_t Read(const uint32_t p_Address);
            void Write(const uint32_t& p_Address, const uint32_t& p_Value);
    };
}

#endif