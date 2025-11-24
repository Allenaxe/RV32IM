#ifndef DATA_MEMORY
#define DATA_MEMORY

#include <cstdint>
#include <bitset>
#include <memory>
#include <optional>

#include "Component.h"
#include "Memory.h"

namespace RV32IM {

    class DataMemory {
        private:
            int32_t imm;
            uint32_t addr;

            uint32_t Load (std::bitset<2> SigMemSize, uint32_t p_Addr, int32_t p_Imm);
            std::nullopt_t Store (uint32_t p_Addr, int32_t p_Imm);

        public:
            DataMemory (std::unique_ptr<Segmentation>& p_Seg);
            std::unique_ptr<Segmentation> seg;      // TODO: should be accept `std::move(p_Seg)` later, where `p_Seg` is a unique_ptr<Segmentation>
            std::optional<uint32_t> Operate (std::bitset<2> MemRW, std::bitset<2> SigMemSize, uint32_t p_Addr, int32_t p_Imm);
    };

}

#endif