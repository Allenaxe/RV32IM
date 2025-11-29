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
        uint32_t data = Storage[ArrayOffset];       // Read in little endian

        return EndianceToggle(data);                // Convert to big endian
    }

    void Segmentation::Write (const uint32_t& p_Address, const uint32_t& p_Value, std::bitset<4> p_ByteMask) {

        std::bitset<4> LE_ByteMask;
        LE_ByteMask[3] = p_ByteMask[0];
        LE_ByteMask[2] = p_ByteMask[1];
        LE_ByteMask[1] = p_ByteMask[2];
        LE_ByteMask[0] = p_ByteMask[3];

        // Reconstruct data to write
        std::bitset<32> OriginalData { this->Read(p_Address) };
        std::bitset<32> TargetData { p_Value };

        std::bitset<32> ResultData("");
        for (int i=0; i<4; i++){
            if (LE_ByteMask[i]) {
                ResultData[i]   = TargetData[i];
                ResultData[i+1] = TargetData[i+1];
                ResultData[i+2] = TargetData[i+2];
                ResultData[i+3] = TargetData[i+3];
            }
            else {
                ResultData[i]   = OriginalData[i];
                ResultData[i+1] = OriginalData[i+1];
                ResultData[i+2] = OriginalData[i+2];
                ResultData[i+3] = OriginalData[i+3];
            }
        }

        uint32_t ArrayOffset = AddrTranslate(p_Address);
        Storage[ArrayOffset] = EndianceToggle(ResultData.to_ulong());           // Convert to little-endian
    };

    // Convert between big endian and little endian
    uint32_t Segmentation::EndianceToggle (uint32_t p_Data) {
        return ((p_Data & 0x0000'00FF) << 24) |
               ((p_Data & 0x0000'FF00) <<  8) |
               ((p_Data & 0x00FF'0000) >>  8) |
               ((p_Data & 0xFF00'0000) >> 24);
    }
}