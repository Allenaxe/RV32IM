#ifndef DATA_MEMORY_H
#define DATA_MEMORY_H

#include <cstdint>
#include <bitset>
#include <memory>
#include <optional>

#include "Structure.h"
#include "Component.h"
#include "Memory.h"
#include "Exception.h"

namespace RV32IM {

    class DataMemory {
        private:
            int32_t imm;
            uint32_t addr;

            // Helper function
            size_t GetTrailingZero (std::bitset<4> p_ByteMask);

            uint32_t Load (int32_t p_Addr, std::bitset<4> p_ByteMask, bool SignExt);
            std::nullopt_t Store (uint32_t p_AlignedAddr, std::bitset<4> p_ByteMask, int32_t p_Imm);

        public:
            DataMemory (std::unique_ptr<Segmentation>& p_Seg);
            std::unique_ptr<Segmentation> seg;
            std::optional<uint32_t> Operate (MEM_RW MemRW, bool SignExt, std::bitset<4> ByteMask, uint32_t p_AlignedAddr, int32_t p_Imm);
    };

}

#endif