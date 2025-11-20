#include "Utility.h"

namespace RV32IM {

     void Utility::Print() {
        for (auto& out : Trace) {
            std::visit(Printer(), out);
        }
    }
}
