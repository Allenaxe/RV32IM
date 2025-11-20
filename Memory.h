#ifndef CPU_MEMORY
#define CPU_MEMORY

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>

namespace RV32IM {
    typedef unsigned char byte;

    class Memory {
        private:
            const uint32_t PHY_LOW_ADDR = 0x0000'0000;
            const uint32_t PHY_HIGH_ADDR = 0x0000'5000;
        
        protected:
            static std::vector<uint32_t> Storage;           // shared memory

        public:
            Memory ();
            ~Memory ();
            void Clear ();

            friend class Segmentation;
    };

    class Segmentation : public Memory {
        private:
            uint32_t AddrTranslate (const uint32_t p_Address);      // helper function

        public:
            const uint32_t START_ADDR;      // text section starts here
            const uint32_t DATA_ADDR;
            const uint32_t BSS_ADDR;
            const uint32_t HEAP_ADDR;
            const uint32_t END_ADDR;        // stack section starts here

            Segmentation(uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength);
            uint32_t Read (const uint32_t p_Address);
            void Write (const uint32_t& p_Address, const uint32_t& p_Value);
    };
    
}

#endif