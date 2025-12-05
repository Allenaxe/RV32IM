#include "ForwardingUnit.h"

namespace RV32IM {

    uint32_t ForwardingUnit::ALUMux(std::bitset<5> p_rs, uint32_t p_op,
            const EX_MEM_Data &EX_MEM, const MEM_WB_Data &MEM_WB) {
        // forwarding op
        std::bitset<2> forwarding_op = ForwardingUnit::ALUForwardingSignal(
            p_rs.to_ulong(),
            EX_MEM.rd, EX_MEM.wb_ctrl.RegWrite,
            MEM_WB.rd, MEM_WB.wb_ctrl.RegWrite
        );
        // if ex hazard
        if(forwarding_op == 0b10) {
            p_op = EX_MEM.alu_result;
        }
        // if mem hazard
        else if(forwarding_op == 0b01) {
            p_op = MEM_WB.wb_ctrl.MemToReg ? MEM_WB.mem_data.value_or(0) : MEM_WB.alu_result;
        }

        return p_op;
    }

    std::bitset<2> ForwardingUnit::ALUForwardingSignal (
        uint8_t EX_rs,
        std::bitset<5> p_EX_MEM_rd, bool p_EX_MEM_RegWrite,
        std::bitset<5> p_MEM_WB_rd, bool p_MEM_WB_RegWrite
    ) {
        // EX hazard
        if (p_EX_MEM_RegWrite &&
            (p_EX_MEM_rd.to_ulong() != 0) &&
            (p_EX_MEM_rd.to_ulong() == EX_rs)) {
            return std::bitset<2> {"10"};
        }

        // MEM hazard
        else if (p_MEM_WB_RegWrite &&
            (p_MEM_WB_rd.to_ulong() != 0) &&
            (p_MEM_WB_rd.to_ulong() == EX_rs)) {
            return std::bitset<2> {"01"};
        }

        return std::bitset<2> {"00"};
    }

}