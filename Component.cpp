#include "Component.h"

namespace RV32IM {
    int32_t SignExtend::extend(uint32_t p_Immediate, uint32_t p_extend) {
        int32_t mask = 1 << (p_extend - 1);
        return (p_Immediate ^ mask) - mask;
    }
}