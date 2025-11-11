#include "Component.h"

namespace RV32IM {
    auto RegisterFile::read(uint8_t rs1, uint8_t rs2) {
        return std::make_pair(Register[rs1], Register[rs2]);
    }

    void RegisterFile::write(uint8_t rd, uint32_t wd, bool we) {
        if (rd != 0 && we == 1)
            Register[rd] = wd;
    }

    int32_t SignExtend::extend(uint32_t p_Immediate, uint32_t p_extend) {
        int32_t mask = 1 << (p_extend - 1);
        return (p_Immediate ^ mask) - mask;
    }
}