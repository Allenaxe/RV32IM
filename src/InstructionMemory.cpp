#include "InstructionMemory.h"

namespace RV32IM {
    InstructionMemory::InstructionMemory () {}

    uint32_t InstructionMemory::FetchInstr (std::unique_ptr<Segmentation>& p_Seg, uint32_t p_PC) {
        return p_Seg -> Read(p_PC);
    }
}