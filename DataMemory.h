#ifndef DATA_MEMORY
#define DATA_MEMORY

#include <cstdint>
#include <bitset>
#include <memory>
#include "Component.h"
#include "Memory.h"

namespace RV32IM {
    class DataMemory {
        private:
            std::unique_ptr<Segmentation> seg;      // TODO: should be accept `std::move(p_Seg)` later, where `p_Seg` is a unique_ptr<Segmentation>
            int32_t imm;
            uint32_t addr;

        public:
            DataMemory (std::unique_ptr<Segmentation>& p_Seg, int32_t p_Imm, uint32_t p_Addr, std::bitset<2> SignalRW);
            uint32_t Load();
            void Store();
    };
}

#endif