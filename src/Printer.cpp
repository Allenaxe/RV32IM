#include "Printer.h"

namespace RV32IM {

    Printer::Printer (const std::string& Filename, bool toConsole): ConsoleOutput(toConsole) {
        LogFile.open(Filename);
        if(!LogFile.is_open()) {
            std::cerr << "[Printer Error] 無法開啟 Log 檔案: " << Filename << '\n';
        }
    }

    Printer::~Printer () {
        if(LogFile.is_open()) {
            LogFile.close();
        }
    }

    void Printer::RecordState (const IF_ID_Data& p_Data) {
        CurrentCycleSnapshot.IF_data = p_Data;
    }

    void Printer::RecordState (const ID_EX_Data& p_Data) {
        CurrentCycleSnapshot.ID_data = p_Data;
    }

    void Printer::RecordState (const EX_MEM_Data& p_Data) {
        CurrentCycleSnapshot.EX_data = p_Data;
    }

    void Printer::RecordState (const MEM_WB_Data& p_Data) {
        CurrentCycleSnapshot.MEM_data = p_Data;
    }

    void Printer::RecordState (const WB_Data& p_Data) {
        CurrentCycleSnapshot.WB_data = p_Data;
    }

    void Printer::EndCycle (int cycle_num) {
        CurrentCycleSnapshot.cycle_number = cycle_num;
        History.push_back(CurrentCycleSnapshot);

        CurrentCycleSnapshot = CycleSnapshot();
    }

    void Printer::PrintTrace () {
        std::ostringstream oss;
        for (const auto& snap : History) {
            oss << "=================================================\n";
            oss << " CYCLE " << snap.cycle_number << "\n";
            oss << "=================================================\n";

            if (snap.IF_data.has_value()) {
                PrintState(oss, *snap.IF_data);
            } else {
                oss << "[IF] Bubble\n";
            }

            if (snap.ID_data.has_value()) {
                PrintState(oss, *snap.ID_data);
            } else {
                oss << "[ID] Bubble\n";
            }

            if (snap.EX_data.has_value()) {
                PrintState(oss, *snap.EX_data);
            } else {
                oss << "[EX] Bubble\n";
            }

            if (snap.MEM_data.has_value()) {
                PrintState(oss, *snap.MEM_data);
            } else {
                oss << "[MEM] Bubble\n";
            }

            if (snap.WB_data.has_value()) {
                PrintState(oss, *snap.WB_data);
            } else {
                oss << "[WB] Bubble\n";
            }

            oss << "\n";
        }
        WriteRaw(oss.str());
    }

    void Printer::PrintState (std::ostream& os, const IF_ID_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[FETCH]\n";
        os << "-------------------------------------------------\n";
        os << "Program Counter: " << out.pc << '\n';
        os << "Instruction    : " << std::bitset<32> {out.inst} << '\n';
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const ID_EX_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[DECODE]\n";
        os << "-------------------------------------------------\n";
        os << "rs1            : " << out.rs1 << '\n';
        os << "rs2            : " << out.rs2 << '\n';
        os << "rd             : " << out.rd << '\n';
        os << "funct3         : " << out.funct3 << '\n';
        os << "funct7         : " << out.funct7 << '\n';
        os << "imm            : " << std::bitset<32>(out.imm) << "\n";
        os << "control signal : " << ControlUnit::SerializeControlSignal(out.control_signal)<< "\n";
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const EX_MEM_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[EXECUTE]\n";
        os << "-------------------------------------------------\n";
        os << "alu result     : " << out.alu_result << '\n';
        os << "write data     : " << out.write_data << '\n';
        os << "rd             : " << out.rd << '\n';
        os << "control signal : " << ControlUnit::SerializeControlSignal(out.control_signal)<< "\n";
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const MEM_WB_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[MEMORY]\n";
        os << "-------------------------------------------------\n";
        os << "mem data       : " << out.mem_data << '\n';
        os << "alu result     : " << out.alu_result << '\n';
        os << "rd             : " << out.rd << '\n';
        os << "control signal : " << ControlUnit::SerializeControlSignal(out.control_signal)<< '\n';
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const WB_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[WRITEBACK]\n";
        os << "-------------------------------------------------\n";
        os << "writeback data : " << out.writeback_data << '\n';
        os << "rd             : " << out.rd << '\n';
        os << "control signal : " << ControlUnit::SerializeControlSignal(out.control_signal)<< '\n';
        os << "-------------------------------------------------\n";
    }

    void Printer::WriteRaw (const std::string& message) {
        if (LogFile.is_open()) {
            LogFile << message;
        }

        if (ConsoleOutput) {
            std::cout << message;
        }
    }
}
