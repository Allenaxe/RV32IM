#ifndef CPU_UTILITY
#define CPU_UTILITY
#include <variant>
#include <fstream>
#include <sstream>
#include <vector>
#include "Structure.h"

namespace RV32IM {

    class Printer {
        private:
            std::vector<CycleSnapshot> History;
            CycleSnapshot CurrentCycleSnapshot;
            std::ofstream LogFile;
            bool ConsoleOutput;
            
        public:
            Printer(const std::string& Filename, bool toConsole);
            ~Printer();

            void RecordState(const IF_ID_Data& p_Data);
            void RecordState(const ID_EX_Data& p_Data);
            void RecordState(const EX_MEM_Data& p_Data);
            void RecordState(const MEM_WB_Data& p_Ddata);
            void RecordState(const WB_Data& p_Ddata);
            void EndCycle(int cycle_num);
            void PrintTrace();

            void PrintState(std::ostream& os, const IF_ID_Data& data);
            void PrintState(std::ostream& os, const ID_EX_Data& data);
            void PrintState(std::ostream& os, const EX_MEM_Data& data);
            void PrintState(std::ostream& os, const MEM_WB_Data& data);
            void PrintState(std::ostream& os, const WB_Data& out);

            void WriteRaw(const std::string& message);
    };
}

#endif