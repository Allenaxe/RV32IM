#include "Memory.h"

namespace RV32IM {
    Segmentation::Segmentation (uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength)
        : START_ADDR(p_startAddr), DATA_ADDR(p_startAddr + p_textLength),
          BSS_ADDR(p_startAddr + 0x0000'0150), HEAP_ADDR(p_startAddr + 0x0000'0200),
          END_ADDR(p_endAddr) {}

    Memory::Memory() {
        const size_t SIZE = (PHY_HIGH_ADDR - PHY_LOW_ADDR) / sizeof(uint32_t);
        m_MemorySpace = new uint32_t[SIZE];
    }

    // Helper function: translate address format to array offset
    uint32_t Memory::AddrTranslate (uint32_t p_Address) {
        return (p_Address - PHY_LOW_ADDR) / sizeof(uint32_t);
    }

    uint32_t Memory::Read (uint32_t p_Address) {
        uint32_t ArrayOffset = AddrTranslate(p_Address);
        return m_MemorySpace[ArrayOffset];
    }

    Memory::~Memory () {
        if( m_MemorySpace != nullptr ) {
            delete[] m_MemorySpace;
            m_MemorySpace = nullptr;
        }
    }

    void Memory::Write (const uint32_t& p_Address, const uint32_t& p_Value) {
        uint32_t ArrayOffset = AddrTranslate(p_Address);
        m_MemorySpace[ArrayOffset] = p_Value;
    };

    void Memory::Clear () {
        for (size_t i = PHY_LOW_ADDR; i < PHY_HIGH_ADDR; ++i) {
            m_MemorySpace[i] = 0;
        }
    }

    uint32_t* Memory::GetAddr() const {
        return m_MemorySpace;
    }

    // Allocate a segmentation
    // Args: p_instrLength is number of instructions
    Segmentation Memory::Allocate(size_t p_instrLength) const {
        // Preserved for dynamic allocation of segmentation
        uint32_t StartAddr = 0x0000'1000;
        uint32_t EndAddr   = 0x0000'2000;
        return Segmentation(StartAddr, EndAddr, p_instrLength*4);
    }
}