#include "Memory.h"

namespace RV32IM {
    std::vector<uint32_t> MainMemory::Storage;

    MainMemory::MainMemory() {
        const size_t SIZE = (PHY_HIGH_ADDR - PHY_LOW_ADDR) / sizeof(uint32_t);
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
        return (p_Address - START_ADDR) / sizeof(uint32_t);
    }

    uint32_t Segmentation::Read (uint32_t p_Address) {
        uint32_t ArrayOffset = AddrTranslate(p_Address);
        return Storage[ArrayOffset];
    }

    void Segmentation::Write (const uint32_t& p_Address, const uint32_t& p_Value) {
        uint32_t ArrayOffset = AddrTranslate(p_Address);
        Storage[ArrayOffset] = p_Value;
    };
}