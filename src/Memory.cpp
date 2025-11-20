#include "Memory.h"

namespace RV32IM {
    MainMemory::MainMemory(): c_MaxAddress(255), m_MemorySpace(new uint32_t[c_MaxAddress]) {}
    
    uint32_t MainMemory::Read(uint32_t p_Address) {
        return m_MemorySpace[p_Address];
    }

    MainMemory::~MainMemory() {
        if( m_MemorySpace != nullptr ) {
            delete[] m_MemorySpace;
            m_MemorySpace = nullptr;
        }
    }

    void MainMemory::Write(const uint32_t& p_Address, const uint32_t& p_Value) {
        m_MemorySpace[p_Address] = p_Value;
    };

    void MainMemory::Clear() {
        for (byte i = 0; i < c_MaxAddress; ++i) {
            m_MemorySpace[i] = 0;
        }
    }
}