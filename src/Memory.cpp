#include "Memory.h"
#include "Exception.h"
#include <iostream>

namespace RV32IM {
    std::vector<uint8_t> MainMemory::Storage;

    MainMemory::MainMemory() {
        const size_t SIZE = (PHY_HIGH_ADDR - PHY_LOW_ADDR) / sizeof(uint8_t);
        Storage.resize(SIZE, 0);
    }

    MainMemory::~MainMemory () {}

    void MainMemory::Clear () {
        for (size_t i = PHY_LOW_ADDR; i < PHY_HIGH_ADDR; ++i) {
            Storage[i] = 0;
        }
    }

    Segmentation::Segmentation (uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength)
        : START_ADDR(p_startAddr), DATA_ADDR(p_startAddr + p_textLength),
          BSS_ADDR(p_startAddr + 0x0000'0150), HEAP_ADDR(p_startAddr + 0x0000'0200),
          END_ADDR(p_endAddr) {}

    // Helper function: translate address format to array offset
    uint32_t Segmentation::AddrTranslate (uint32_t p_Address) {
        return (p_Address - START_ADDR) / sizeof(uint8_t);
    }

    uint32_t Segmentation::Read (uint32_t p_Address) {

        // Check if target address is readable
        if (p_Address < START_ADDR || p_Address > END_ADDR) {
            std::cout << p_Address << '\n';
            std::string message = std::format("This address {} is not readable.", p_Address);
            throw SegmentationError(message);
        }

        uint32_t ArrayOffset = AddrTranslate(p_Address);
        const uint8_t* mem = &Storage[ArrayOffset];

        return (static_cast<uint32_t>(mem[0]) << 0)  |
               (static_cast<uint32_t>(mem[1]) << 8)  |
               (static_cast<uint32_t>(mem[2]) << 16) |
               (static_cast<uint32_t>(mem[3]) << 24) ;
    }

    void Segmentation::Write (const uint32_t& p_Address, const uint32_t& p_Value, std::bitset<4> p_ByteMask) {
        uint32_t ArrayOffset = AddrTranslate(p_Address);
        uint8_t* mem = &Storage[ArrayOffset];

        if(p_ByteMask[0]) mem[0] = (p_Value >> 0)  & 0xFF;
        if(p_ByteMask[1]) mem[1] = (p_Value >> 8)  & 0xFF;
        if(p_ByteMask[2]) mem[2] = (p_Value >> 16) & 0xFF;
        if(p_ByteMask[3]) mem[3] = (p_Value >> 24) & 0xFF;
    };
}