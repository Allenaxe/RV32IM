#include "DataMemory.h"
#include "Exception.h"

namespace RV32IM {
    DataMemory::DataMemory (std::unique_ptr<Segmentation>& p_Seg) : seg(std::move(p_Seg)) {}

    std::optional<uint32_t> DataMemory::Operate (std::bitset<2> MemRW, std::bitset<2> SigMemSize, uint32_t p_Addr, int32_t p_Imm){
        /********************
         * MemRW:
         *      - MemRW[0]: allow to read memory if this bit is set
         *      - MemRW[1]: allow to write to memory if this bit is set
         ********************/

        if (MemRW[0])
            return Load(SigMemSize, p_Imm, p_Addr);
        else if (MemRW[1])
            return Store(p_Addr, p_Imm);

        return std::nullopt;
    }

    // TODO: LBU & LHU are not implemented
    uint32_t DataMemory::Load (std::bitset<2> SigMemSize, uint32_t p_Addr, int32_t p_Imm) {
        uint32_t data = seg -> Read(p_Addr);

        switch (SigMemSize.to_ulong()){

            case 0b00:          // byte
                return (data & 0xFFFFFF00);

            case 0b01:          // half word
                return (data & 0xFFFF0000);

            case 0b10:          // word
                return data;

            case 0b11:          // reserved
                throw ValueError("DataMemory: 0b00 of `SigMemSize` is reserved.");

            default:
                throw ValueError("DataMemory: `SigMemSize` didn't matched.");
        }
        return seg -> Read(addr);
    }

    std::nullopt_t DataMemory::Store (uint32_t p_Addr, int32_t p_Imm) {
        seg -> Write(p_Addr, p_Imm);
        return std::nullopt;
    }
}