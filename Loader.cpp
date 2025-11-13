#include "Loader.h"
#include "Memory.h"

#define BASE_ADDR 0x00000000

namespace RV32IM {
    Loader::Loader() {};

    std::unique_ptr<Segmentation> Loader::Load(std::string p_filename, Memory &p_memory){     // load from text file currently
        startAddr = BASE_ADDR;              // starting addrees was fixed to 0x0
        std::fstream FileIn(p_filename, std::ios::in);
        std::string strLine;

        // Put text section into buffer
        std::vector<uint32_t> InstrBuffer;
        size_t numInstr = 0;
        while(getline(FileIn, strLine)){
            if (strLine.empty()) continue;

            std::bitset<32> bitLine(strLine);
            std::uint32_t value = static_cast<std::uint32_t>(bitLine.to_ulong());
            InstrBuffer.push_back(value);
            numInstr++;
        }

        Segmentation mySegment = p_memory.Allocate(numInstr);

        std::uint32_t Offset = 0;

        for (auto it : InstrBuffer) {
            p_memory.Write(mySegment.START_ADDR + Offset, it);
            Offset += 4;        // sizeof(uint32_t) = 4
        }

        for(uint32_t idx = 0; idx < Offset; idx+=4){
            std::cout << static_cast<std::bitset<32>>(p_memory.Read(mySegment.START_ADDR + idx)) << std::endl;
        }

        return std::make_unique<Segmentation>(mySegment);
    }
}