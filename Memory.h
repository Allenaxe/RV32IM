#ifndef CPU_MEMORY
#define CPU_MEMORY

#include <cstdint>
#include <cstddef>
#include "Memory.h"

namespace RV32IM {
    typedef unsigned char byte;

    class Segmentation {
        public:
            const uint32_t START_ADDR;  // text section starts here
            const uint32_t DATA_ADDR;
            const uint32_t BSS_ADDR;
            const uint32_t HEAP_ADDR;
            const uint32_t END_ADDR;    // stack section starts here

            Segmentation(uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength);
    };

    class Memory {
        private:     // changed to privete after debug
            const uint32_t PHY_LOW_ADDR = 0x0000'0000;
            const uint32_t PHY_HIGH_ADDR = 0x0000'5000;
            // const uint32_t c_MaxAddress;
            uint32_t* m_MemorySpace;
            uint32_t AddrTranslate (const uint32_t p_Address);

        public:
            Memory ();
            ~Memory ();
            void Clear ();
            uint32_t Read (const uint32_t p_Address);
            void Write (const uint32_t& p_Address, const uint32_t& p_Value);
            uint32_t* GetAddr() const;
            Segmentation Allocate(size_t p_instrLength) const;
    };
}

#endif