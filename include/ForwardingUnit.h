#ifndef FORWARD_UNIT_H
#define FORWARD_UNIT_H

#include <cstdint>
#include <bitset>

#include "Structure.h"

namespace RV32IM {

    class ForwardingUnit {
        public:
            static uint32_t ALUMux (std::bitset<5> p_rs,
                                    uint32_t p_op,
                                    const EX_MEM_Data &EX_MEM,
                                    const MEM_WB_Data &MEM_WB);

        private:
            static std::bitset<2> ALUForwardingSignal (uint8_t EX_rs,
                                                       std::bitset<5> p_EX_MEM_rd,
                                                       bool p_EX_MEM_RegWrite,
                                                       std::bitset<5> p_MEM_WB_rd,
                                                       bool p_MEM_WB_RegWrite);
    };

}

#endif