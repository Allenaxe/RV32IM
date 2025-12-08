#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <cstdint>
#include <bitset>
#include <memory>

#include "PipelineRegister.h"
#include "Component.h"
#include "Structure.h"
#include "Printer.h"
#include "Memory.h"
#include "LoadStoreUnit.h"
#include "DataMemory.h"
#include "InstructionMemory.h"

namespace RV32IM {

    class CPU {
        private:
            // Registers
            uint32_t PC;                            // Program Counter
            uint32_t MAR;                           // Memory Address Register
            uint32_t MDR;                           // Memory Data Register
            uint32_t IR;                            // Instruction Register

            PipelineRegister<IF_ID_Data>  IF_ID;
            PipelineRegister<ID_EX_Data>  ID_EX;
            PipelineRegister<EX_MEM_Data> EX_MEM;
            PipelineRegister<MEM_WB_Data> MEM_WB;

            RegisterFile* RF;

            // bool m_OverflowError;
            // bool m_UnderflowError;
            // bool m_SignedMode;
            // bool m_Halt;

            // Segmentation
            DataMemory* DM;             // Program Segmentation

            // Component
            InstructionMemory InstrMem;

            // Stages
            void Fetch();
            ID_EX_Data Decode(IF_ID_Data& p_DecodeInput);
            EX_MEM_Data Execute(ID_EX_Data& p_ExecuteInput);
            MEM_WB_Data Memory(EX_MEM_Data& p_MemoryInput);
            WB_Data WriteBack(MEM_WB_Data& p_WriteBackInput);

            // Utility
            Printer* Record;

        public:
            CPU(std::unique_ptr<Segmentation>& p_ProgSeg,
                std::string p_Filename,
                std::string p_TableFilename,
                bool p_ConsoleOutput);
            // ~CPU();
            // void Reset();
            void Run();
    };

}
#endif