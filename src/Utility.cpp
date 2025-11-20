#include "Utility.h"

namespace RV32IM {

    void Utility::Record(Output p_Output) {
        Trace.push_back(p_Output);
    }

    void Utility::Print() {
        for (auto& out : Trace) {
            std::visit(Printer(), out);
        }
    }
}
