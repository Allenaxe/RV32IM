#include "Memory.h"

namespace RV32IM {
    Memory::Memory(): c_MaxAddress(255), m_MemorySpace(new uint32_t[c_MaxAddress]) {}
        uint32_t Memory::Read(uint32_t p_Address) {
        return m_MemorySpace[p_Address];
    }

    Memory::~Memory() {
        if( m_MemorySpace != nullptr ) {
            delete[] m_MemorySpace;
            m_MemorySpace = nullptr;
        }
    }

    void Memory::Write(const uint32_t& p_Address, const uint32_t& p_Value) {
        m_MemorySpace[p_Address] = p_Value;
    };

    void Memory::Clear() {
        for (byte i = 0; i < c_MaxAddress; ++i) {
            m_MemorySpace[i] = 0;
        }
    }
}