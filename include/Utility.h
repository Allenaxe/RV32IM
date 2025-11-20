#ifndef CPU_UTILITY
#define CPU_UTILITY
#include <variant>
#include <vector>
#include "Structure.h"

namespace RV32IM {

    class Utility {
        private:
            std::vector<CycleSnapshot> History;
            CycleSnapshot CurrentCycleSnapshot;
            
        public:
            void RecordState(const IF_ID_Data& p_Data);
            void RecordState(const ID_EX_Data& p_Data);
            void RecordState(const EX_MEM_Data& p_Data);
            void RecordState(const MEM_WB_Data& p_Ddata);
            void RecordState(const WB_Data& p_Ddata);
            void EndCycle(int cycle_num);
            void PrintTrace();
    };
}

#endif