#include "LoadStoreUnit.h"

namespace RV32IM {

    std::tuple<uint32_t, std::bitset<4>> LoadStoreUnit::DecodeAddr(uint32_t p_Addr, MEM_SIZE MemSize) {
        std::bitset<4> ByteMask;

        switch (MemSize) {

            case MEM_SIZE::BYTE :
                // Determine in big-endian
                switch (p_Addr & 0b11) {
                    case 0b00:
                        ByteMask = std::bitset<4>("0001");      // Forth byte, the lowest byte
                        break;

                    case 0b01:
                        ByteMask = std::bitset<4>("0010");      // Third byte
                        break;

                    case 0b10:
                        ByteMask = std::bitset<4>("0100");      // Second byte
                        break;

                    case 0b11:
                        ByteMask = std::bitset<4>("1000");      // First byte, the highest byte
                        break;
                }
                break;

            case MEM_SIZE::HALF :
                switch (p_Addr & 0b10) {
                    case 0b00:
                        ByteMask = std::bitset<4>("0011");      // Lower half word
                        break;

                    case 0b10:
                        ByteMask = std::bitset<4>("1100");      // Upper half word
                        break;

                    default:
                        throw MemoryError("MemoryError: Requesting address is misaligned.");
                        break;
                }
                break;

            case MEM_SIZE::WORD :
                ByteMask = std::bitset<4>("1111");
                break;

        }

        // Clear lowest two bits to get the aligned address
        uint32_t Aligned4_Addr = p_Addr & 0b00;

        return std::make_tuple(Aligned4_Addr, ByteMask);
    }

}