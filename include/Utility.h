
#ifndef CPU_UTILITY
#define CPU_UTILITY
#include <variant>
#include <vector>
#include "Structure.h"

namespace RV32IM {

    using Output = std::variant<ID_EX_Data/*, ExecuteOutput, MemoryOutput, WritebackOutput*/>;

    class Utility {
        private:
            std::vector<Output> Trace;
            
        public:
            void Print();
    };
}

#endif