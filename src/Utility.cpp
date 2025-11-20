#include "Utility.h"

namespace RV32IM {

    void Utility::RecordState(const IF_ID_Data& p_Data) {
        CurrentCycleSnapshot.IF_data = p_Data;
    }

    void Utility::RecordState(const ID_EX_Data& p_Data) {
        CurrentCycleSnapshot.ID_data = p_Data;
    }

    void Utility::RecordState(const EX_MEM_Data& p_Data) {
        CurrentCycleSnapshot.EX_data = p_Data;
    }

    void Utility::RecordState(const MEM_WB_Data& p_Data) {
        CurrentCycleSnapshot.MEM_data = p_Data;
    }

    void Utility::RecordState(const WB_Data& p_Data) {
        CurrentCycleSnapshot.WB_data = p_Data;
    }

    void Utility::EndCycle(int cycle_num) {
        CurrentCycleSnapshot.cycle_number = cycle_num;
        History.push_back(CurrentCycleSnapshot);
            
        CurrentCycleSnapshot = CycleSnapshot(); 
    }

    void Utility::PrintTrace() {
        Printer printer;
        for (const auto& snap : History) {
            std::cout << "=================================================\n";
            std::cout << " CYCLE " << snap.cycle_number << "\n";
            std::cout << "=================================================\n";
            
            if (snap.IF_data.has_value()) {
                printer(*snap.IF_data);
            } else {
                std::cout << "[IF] Bubble\n";
            }

            if (snap.ID_data.has_value()) {
                printer(*snap.ID_data);
            } else {
                std::cout << "[ID] Bubble\n";
            }

            if (snap.EX_data.has_value()) {
                printer(*snap.EX_data);
            } else {
                std::cout << "[EX] Bubble\n";
            }

            if (snap.MEM_data.has_value()) {
                printer(*snap.MEM_data);
            } else {
                std::cout << "[MEM] Bubble\n";
            }

            if (snap.WB_data.has_value()) {
                printer(*snap.WB_data);
            } else {
                std::cout << "[WB] Bubble\n";
            }
         
            std::cout << "\n";
        }
    }
}
