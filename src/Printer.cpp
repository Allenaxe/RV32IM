#include "Printer.h"

namespace RV32IM {

    std::ostream& operator<< (std::ostream& os, MEM_RW size) {
        switch(size) {
            case MEM_RW::NOP: os << "NOP"; break;
            case MEM_RW::READ: os << "READ"; break;
            case MEM_RW::WRITE: os << "WRITE"; break;
        }
        return os;
    }

    std::ostream& operator<< (std::ostream& os, MEM_SIZE size) {
        switch(size) {
            case MEM_SIZE::BYTE: os << "BYTE"; break;
            case MEM_SIZE::HALF: os << "HALF"; break;
            case MEM_SIZE::WORD: os << "WORD"; break;
        }
        return os;
    }

    std::ostream& operator<< (std::ostream& os, ALU_OP_TYPE t) {
        switch(t) {
            case ALU_OP_TYPE::MEMORY_REF: os << "MEMORY_REF"; break;
            case ALU_OP_TYPE::BRANCH:     os << "BRANCH";     break;
            case ALU_OP_TYPE::R_TYPE:     os << "R_TYPE";     break;
            case ALU_OP_TYPE::I_TYPE:     os << "I_TYPE";     break;
            case ALU_OP_TYPE::LUI:        os << "LUI";        break;
            case ALU_OP_TYPE::AUIPC:      os << "AUIPC";      break;
        }
        return os;
    }

    Printer::Printer (const std::string& Filename, const std::string& TableFilename, bool toConsole): ConsoleOutput(toConsole) {
        LogFile.open(Filename);
        if(!LogFile.is_open()) {
            std::cerr << "[Printer Error] Cannot open log file: " << Filename << '\n';
        }

        TableLogFile.open(TableFilename);
        if(!LogFile.is_open()) {
            std::cerr << "[Printer Error] Cannot open log file: " << TableFilename << '\n';
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

    void Printer::PrintRegisters (RegisterFile* RF) {
        std::ostringstream oss;
        const std::string regNames[32] = {
            "x0/zero", "x1/ra", "x2/sp", "x3/gp", "x4/tp", "x5/t0", "x6/t1", "x7/t2",
            "x8/s0", "x9/s1", "x10/a0", "x11/a1", "x12/a2", "x13/a3", "x14/a4", "x15/a5",
            "x16/a6", "x17/a7", "x18/s2", "x19/s3", "x20/s4", "x21/s5", "x22/s6", "x23/s7",
            "x24/s8", "x25/s9", "x26/s10", "x27/s11", "x28/t3", "x29/t4", "x30/t5", "x31/t6"
        };

        oss << "--------------------------------------------\n";
        oss << "| Register |  Hex Value  |  Decimal Value  |\n";
        oss << "--------------------------------------------\n";

        for(uint8_t i = 0; i < 32; i = i + 2) {

            RegisterFileRead RF_read = RF->Read(i, i + 1);

            oss << "| "
                    << std::setw(8) << regNames[i] << " |  0x"
                    << std::setw(8) << std::setfill('0') << std::hex << RF_read.rs1
                    << " | " << std::setw(15) << std::setfill(' ') << std::dec << RF_read.rs1 << " |\n";

            oss << "| "
                    << std::setw(8) << regNames[i+1] << " |  0x"
                    << std::setw(8) << std::setfill('0') << std::hex << RF_read.rs2
                    << " | " << std::setw(15) << std::setfill(' ') << std::dec << RF_read.rs2 << " |\n";
        }
        oss << "--------------------------------------------\n";
        WriteRaw(oss.str());
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
        os << "Program Counter                        : " << out.pc << '\n';
        os << "Instruction                            : " << std::bitset<32> {out.inst} << '\n';
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const ID_EX_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[DECODE]\n";
        os << "-------------------------------------------------\n";
        os << "rs1                                    : " << out.rs1 << '\n';
        os << "rs2                                    : " << out.rs2 << '\n';
        os << "rd                                     : " << out.rd << '\n';
        os << "funct3                                 : " << out.funct3 << '\n';
        os << "funct7                                 : " << out.funct7 << '\n';
        os << "imm                                    : " << std::bitset<32>(out.imm) << "\n";
        os << "{ ALUSrc, Branch, Jump, ALUOp }        : " << "{ " << out.ex_ctrl.ALUSrc << ", " << out.ex_ctrl.Branch << ", " << out.ex_ctrl.Jump << ", " << out.ex_ctrl.ALUOp << " }\n";
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const EX_MEM_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[EXECUTE]\n";
        os << "-------------------------------------------------\n";
        os << "alu result                             : " << out.alu_result << '\n';
        os << "write data                             : " << out.write_data << '\n';
        os << "rd                                     : " << out.rd << '\n';
        os << "{ MemRead, MemWrite, Signext, MemSize }: " << "{ " << out.mem_ctrl.MemRW << ", " << out.mem_ctrl.SignExt << ", " << out.mem_ctrl.MemSize << " }\n";
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const MEM_WB_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[MEMORY]\n";
        os << "-------------------------------------------------\n";
        os << "mem data                               : " << out.mem_data.value_or(0) << '\n';
        os << "alu result                             : " << out.alu_result << '\n';
        os << "rd                                     : " << out.rd << '\n';
        os << "{ RegWrite, MemtoReg }                 : " << "{ " << out.wb_ctrl.MemToReg << ", " << out.wb_ctrl.RegWrite << " }\n";
        os << "-------------------------------------------------\n";
    }

    void Printer::PrintState (std::ostream& os, const WB_Data& out) {
        os << "-------------------------------------------------\n";
        os << "[WRITEBACK]\n";
        os << "-------------------------------------------------\n";
        os << "writeback data                         : " << out.writeback_data << '\n';
        os << "rd                                     : " << out.rd << '\n';
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

    void Printer::PrintTable () {
        // Use tabulate datatypes here
        using namespace tabulate;

        Table table;

        size_t numCycle = History.size();

        // Add column headers
        Table::Row_t header;
        header.push_back("");
        for (size_t i=0; i<numCycle; i++) {
            header.push_back( std::format("Cycle {}", i+1) );
        }
        table.add_row(header);

        // Add row headers
        std::vector<std::string> StageName = {"IF", "ID", "EXE", "MEM", "WB"};
        for (size_t i=0; i<StageName.size(); i++) {
            table.add_row( {StageName[i]} );
        }

        // Fill snap into each cell
        unsigned int ClockIdx = 0;
        std::ostringstream oss;
        for (const auto& snap : History) {
            ClockIdx++;

            // IF stage
            oss.str("");
            if (snap.IF_data.has_value()) {
                auto out = snap.IF_data.value();
                oss << "Program Counter                        : " << out.pc << '\n';
                oss << "Instruction                            : " << std::bitset<32> {out.inst} << '\n';
            }
            else {
                oss << "[IF] Bubble\n";
            }
            table.row(1)[ClockIdx].set_text(oss.str());

            // ID stage
            oss.str("");
            if (snap.ID_data.has_value()) {
                auto out = snap.ID_data.value();
                oss << "rs1                                    : " << out.rs1 << '\n';
                oss << "rs2                                    : " << out.rs2 << '\n';
                oss << "rd                                     : " << out.rd << '\n';
                oss << "funct3                                 : " << out.funct3 << '\n';
                oss << "funct7                                 : " << out.funct7 << '\n';
                oss << "imm                                    : " << std::bitset<32>(out.imm) << "\n";
                oss << "{ ALUSrc, Branch, Jump, ALUOp }        : " << "{ " << out.ex_ctrl.ALUSrc << ", " << out.ex_ctrl.Branch << ", " << out.ex_ctrl.Jump << ", " << out.ex_ctrl.ALUOp << " }\n";
            }
            else {
                oss << "[ID] Bubble\n";
            }
            table.row(2)[ClockIdx].set_text(oss.str());

            // EXE stage
            oss.str("");
            if (snap.EX_data.has_value()) {
                auto out = snap.EX_data.value();
                oss << "alu result                             : " << out.alu_result << '\n';
                oss << "write data                             : " << out.write_data << '\n';
                oss << "rd                                     : " << out.rd << '\n';
                oss << "{ MemRead, MemWrite, Signext, MemSize }: " << "{ " << out.mem_ctrl.MemRW << ", " << out.mem_ctrl.SignExt << ", " << out.mem_ctrl.MemSize << " }\n";
            }
            else {
                oss << "[EX] Bubble\n";
            }
            table.row(3)[ClockIdx].set_text(oss.str());

            // MEM stage
            oss.str("");
            if (snap.MEM_data.has_value()) {
                auto out = snap.MEM_data.value();
                oss << "mem data                               : " << out.mem_data.value_or(0) << '\n';
                oss << "alu result                             : " << out.alu_result << '\n';
                oss << "rd                                     : " << out.rd << '\n';
                oss << "{ RegWrite, MemtoReg }                 : " << "{ " << out.wb_ctrl.MemToReg << ", " << out.wb_ctrl.RegWrite << " }\n";
            }
            else {
                oss << "[MEM] Bubble\n";
            }
            table.row(4)[ClockIdx].set_text(oss.str());

            // WB stage
            oss.str("");
            if (snap.WB_data.has_value()) {
                auto out = snap.WB_data.value();
                oss << "writeback data                         : " << out.writeback_data << '\n';
                oss << "rd                                     : " << out.rd << '\n';
            }
            else {
                oss << "[WB] Bubble\n";
            }
            table.row(5)[ClockIdx].set_text(oss.str());
        }

        table.format()
             .font_style({FontStyle::bold})
             .border_top("-")
             .border_bottom("-")
             .border_left("|")
             .border_right("|")
             .corner("+");

        table.row(0).format()
                    .font_align(FontAlign::center);

        // Make row header aligns vertically center
        for (int rowIdx = 1; rowIdx<table.size(); rowIdx++){

            // Determine the max height of each row
            auto& row = table.row(rowIdx);
            size_t row_height = 0;
            for (size_t i=0; i<row.size(); i++){
                const std::string& ops_text = row[i].get_text();
                const size_t cell_height = std::count(ops_text.begin(), ops_text.end(), '\n');
                row_height = std::max(row_height, cell_height);
            }

            // If row height is even, add a line of bottom padding to that row
            if (row_height % 2 == 0) {
                row.format().padding_bottom(1);
            }

            // Align row header to vertically center
            row[0].set_text(std::string(row_height / 2, '\n') + row[0].get_text());
        }


        TableLogFile << table;
        TableLogFile.flush();
    }

}
