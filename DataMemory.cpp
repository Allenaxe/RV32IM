#include "DataMemory.h"
#include <iostream>
using namespace std;

namespace RV32IM {
    DataMemory::DataMemory (std::unique_ptr<Segmentation>& p_Seg, int32_t p_Imm, uint32_t p_Addr, std::bitset<2> SignalRW)
                            : imm(p_Imm), addr(p_Addr) {
        // TODO: use unique_ptr instead, and use std::move to transfer ownership
    }

    // TODO: haven't implemented lh, lb, sh, sb
    uint32_t DataMemory::Load () {
        return seg -> Read(addr);
    }

    void DataMemory::Store () {
        return seg -> Write(addr, imm);
    }
}