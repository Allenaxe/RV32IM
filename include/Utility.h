
#ifndef CPU_UTILITY
#define CPU_UTILITY
#include <variant>
#include <vector>
#include "Structure.h"

namespace RV32IM {

    using Output = std::variant<IF_ID_Data, ID_EX_Data, EX_MEM_Data, MEM_WB_Data>;

    class Utility {
        private:
            std::vector<Output> Trace;
            
        public:
            void Record(Output p_Output);
            void Print();
    };
}

#endif