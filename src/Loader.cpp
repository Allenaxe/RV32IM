#include "Loader.h"
#include "Memory.h"

#define BASE_ADDR 0x00000000

namespace RV32IM {

    Loader::Loader() {};

    std::unique_ptr<Segmentation> Loader::Load (std::string p_filename, MainMemory &p_memory) {     // load from text file currently
        startAddr = BASE_ADDR;              // Starting addrees was fixed to 0x0
        std::fstream FileIn(p_filename, std::ios::in);
        std::string strLine;

        // Put text section into buffer
        std::vector<uint32_t> InstrBuffer;
        size_t numInstr = 0;
        while(getline(FileIn, strLine)){

            const char* typeOfWhitespaces = " \t\n\r\f\v";
            strLine.erase(0, strLine.find_first_not_of(typeOfWhitespaces));
            strLine.erase(strLine.find_last_not_of(typeOfWhitespaces) + 1);

            if (strLine.empty()) continue;

            std::bitset<32> bitLine(strLine);
            std::uint32_t value = static_cast<std::uint32_t>(bitLine.to_ulong());
            InstrBuffer.push_back(value);
            numInstr++;
        }

        // Allocate a segmentation
        // Args: p_instrLength is number of instructions
        uint32_t StartAddr = 0x0000'1000;               // Note: Preserved for dynamic allocation of segmentation
        uint32_t EndAddr   = 0x0000'2000;
        Segmentation mySeg = Segmentation(StartAddr, EndAddr, numInstr*4);

        std::uint32_t Offset = 0;

        for (auto it : InstrBuffer) {
            mySeg.Write(mySeg.START_ADDR + Offset, it);
            Offset += 4;        // sizeof(uint32_t) = 4
        }

        return std::make_unique<Segmentation>(mySeg);
    }

}