#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>
#include <bitset>
#include <string>
// #include <format>

namespace RV32IM {

    typedef unsigned char byte;

    class MainMemory {
        private:
            const uint32_t PHY_LOW_ADDR  = 0x0000'0000;
            const uint32_t PHY_HIGH_ADDR = 0x0000'5000;

        protected:
            static std::vector<uint8_t> Storage;           // Shared memory

        public:
            MainMemory ();
            ~MainMemory ();
            void Clear ();

            friend class Segmentation;
    };

    class Segmentation : public MainMemory {
        private:
            // Helper functions
            uint32_t AddrTranslate (const uint32_t p_Address);

        public:
            const uint32_t START_ADDR;      // Text section starts here
            const uint32_t DATA_ADDR;
            const uint32_t BSS_ADDR;
            const uint32_t HEAP_ADDR;
            const uint32_t END_ADDR;        // Stack section starts here

            Segmentation(uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength);
            uint32_t Read (const uint32_t p_Address);
            void Write (const uint32_t& p_Address,
                        const uint32_t& p_Value,
                        std::bitset<4> p_ByteMask = std::bitset<4>("1111"));
    };

}

#endif