#include "DataMemory.h"

namespace RV32IM {
    DataMemory::DataMemory (std::unique_ptr<Segmentation>& p_Seg) : seg(std::move(p_Seg)) {}

    std::optional<uint32_t> DataMemory::Operate (MemRW_t p_MemRW, bool p_SignExt, std::bitset<4> p_ByteMask, uint32_t p_Aligned4_Addr, int32_t p_Imm){
        switch (p_MemRW) {
            case MemRW_t::NOP :
                return std::nullopt;

            case MemRW_t::READ :
                return Load(p_Aligned4_Addr, p_ByteMask, p_SignExt);

            case MemRW_t::WRITE :
                return Store(p_Aligned4_Addr, p_ByteMask, p_Imm);
        }

        return std::nullopt;
    }

    uint32_t DataMemory::Load (int32_t p_Addr, std::bitset<4> p_ByteMask, bool SignExt) {
        uint8_t ByteMask = p_ByteMask.to_ulong();

        uint32_t FullByteMask = 0x00000000;
        if (ByteMask & 0b0001) FullByteMask |= 0x000000FF;
        if (ByteMask & 0b0010) FullByteMask |= 0x0000FF00;
        if (ByteMask & 0b0100) FullByteMask |= 0x00FF0000;
        if (ByteMask & 0b1000) FullByteMask |= 0xFF000000;


        uint32_t data = seg -> Read(p_Addr);
        data &= FullByteMask;

        int TrailingZero = GetTrailingZero(p_ByteMask);

        data >>= (TrailingZero * 8);

        if (SignExt) {
            int32_t mask = 1 << (TrailingZero * 8 - 1);
            data = (data ^ mask) - mask;
        }

        return data;
    }

    std::nullopt_t DataMemory::Store (uint32_t p_Aligned4_Addr, std::bitset<4> p_ByteMask, int32_t p_Imm) {
        size_t TraingZero = GetTrailingZero(p_ByteMask);

        seg -> Write(p_Aligned4_Addr + TraingZero*8 , p_Imm, p_ByteMask);

        return std::nullopt;
    }

    size_t DataMemory::GetTrailingZero (std::bitset<4> p_ByteMask) {
        size_t TrailingZero = 0;
        for (int i=0; i<4; i++){
            if (p_ByteMask[i] == 0) TrailingZero++;
            else break;
        }

        return TrailingZero;
    }
}