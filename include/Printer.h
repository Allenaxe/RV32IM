#ifndef PRINTER_H
#define PRINTER_H

#include <variant>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <string>
#include <format>
#include <algorithm>
#include <ranges>

#include <tabulate/table.hpp>

#include "Structure.h"
#include "Component.h"

namespace RV32IM {

    std::ostream& operator<<(std::ostream& os, MEM_RW size);
    std::ostream& operator<<(std::ostream& os, MEM_SIZE size);
    std::ostream& operator<<(std::ostream& os, ALU_OP_TYPE t);

    class Printer {
        private:
            std::vector<CycleSnapshot> History;
            CycleSnapshot CurrentCycleSnapshot;
            std::ofstream TableLogFile;

        public:
            Printer(const std::string& TableFilename);
            ~Printer();

            void RecordState(const IF_ID_Data& p_Data);
            void RecordState(const ID_EX_Data& p_Data);
            void RecordState(const EX_MEM_Data& p_Data);
            void RecordState(const MEM_WB_Data& p_Ddata);
            void RecordState(const WB_Data& p_Ddata);
            void RecordState (RegisterFile* RF);
            void RecordClearSignals (bool IF_ID_Clear, bool ID_EX_Clear);
            void EndCycle(int cycle_num);
            void PrintTable ();
    };

}

#endif