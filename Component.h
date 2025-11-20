#ifndef CPU_COMPONENT
#define CPU_COMPONENT

#include <stdint.h>
#include <memory>
#include "Memory.h"

namespace RV32IM {
    class SignExtend {
        public:
            static int32_t extend(uint32_t p_Immediate, uint32_t p_extend);
    };
}

#endif