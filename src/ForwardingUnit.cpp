#include "ForwardingUnit.h"

namespace RV32IM {

    uint32_t ForwardingUnit::ALUMux (std::bitset<5> p_rs,
                                     uint32_t p_op,
                                     const EX_MEM_Data& EX_MEM,
                                     const MEM_WB_Data& MEM_WB)
    {
        // Forwarding op
        std::bitset<2> forwarding_op = ForwardingUnit::ALUForwardingSignal(
            p_rs.to_ulong(),
            EX_MEM.rd, EX_MEM.wb_ctrl.RegWrite,
            MEM_WB.rd, MEM_WB.wb_ctrl.RegWrite
        );

        switch (forwarding_op.to_ulong()) {
            case 0b10:          // For EX hazard
                p_op = EX_MEM.alu_result;
                break;

            case 0b01:          // For Mem hazard
                p_op = MEM_WB.wb_ctrl.MemToReg ? MEM_WB.mem_data.value_or(0) : MEM_WB.alu_result;
                break;
        }

        return p_op;
    }

    std::bitset<2> ForwardingUnit::ALUForwardingSignal (uint8_t EX_rs,
                                                        std::bitset<5> p_EX_MEM_rd,
                                                        bool p_EX_MEM_RegWrite,
                                                        std::bitset<5> p_MEM_WB_rd,
                                                        bool p_MEM_WB_RegWrite)
    {
        bool ex_hazard = p_EX_MEM_RegWrite &&
                         (p_EX_MEM_rd.to_ulong() != 0) &&
                         (p_EX_MEM_rd.to_ulong() == EX_rs);

        if (ex_hazard) {
            return std::bitset<2> {"10"};
        }

        bool mem_hazard = p_MEM_WB_RegWrite &&
                          (p_MEM_WB_rd.to_ulong() != 0) &&
                          (p_MEM_WB_rd.to_ulong() == EX_rs);

        if (mem_hazard) {
            return std::bitset<2> {"01"};
        }

        // No hazard occured
        return std::bitset<2> {"00"};
    }

}