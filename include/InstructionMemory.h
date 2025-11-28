#ifndef INSTR_MEM_H
#define INSTR_MEM_H

#include <cstdint>
#include <memory>

#include "Memory.h"

namespace RV32IM {

    class InstructionMemory {
        public:
            InstructionMemory ();
            uint32_t FetchInstr (std::unique_ptr<Segmentation>& p_Seg, uint32_t p_PC);
    };
}




#endif