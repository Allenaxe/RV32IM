#ifndef LOAD_STORE_UNIT_H
#define LOAD_STORE_UNIT_H

#include <cstdint>
#include <bitset>
#include <tuple>

#include "Structure.h"
#include "Exception.h"

namespace RV32IM {
    class LoadStoreUnit {
        public:
            // Generate a tuple (AlignedAddr, ByteMask)
            static std::tuple<uint32_t, std::bitset<4>> DecodeAddr(uint32_t p_Addr, MEM_SIZE MemSize);
    };
}

#endif