#include "Utility.h"

namespace RV32IM {

    void Utility::Record(Output p_Output) {
        Trace.push_back(p_Output);
    }

    void Utility::Print() {
        for (int i = 1; i < Trace.size() ; ++i) {
            std::cout << "Cycle: " << i << '\n';
            std::visit(Printer(), Trace[i]);
        }
    }
}
