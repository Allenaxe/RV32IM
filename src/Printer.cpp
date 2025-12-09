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
            case ALU_OP_TYPE::MEMORY_REF:  os << "MEMORY_REF";  break;
            case ALU_OP_TYPE::BRANCH:      os << "BRANCH";      break;
            case ALU_OP_TYPE::R_TYPE:      os << "R_TYPE";      break;
            case ALU_OP_TYPE::I_TYPE:      os << "I_TYPE";      break;
            case ALU_OP_TYPE::LUI:         os << "LUI";         break;
            case ALU_OP_TYPE::AUIPC:       os << "AUIPC";       break;
            case ALU_OP_TYPE::M_Extension: os << "M_EXTENSION"; break;
        }
        return os;
    }

    Printer::Printer (const std::string& TableFilename) {
        TableLogFile.open(TableFilename);
        if(!TableLogFile.is_open()) {
            std::cerr << "[Printer Error] Cannot open log file: " << TableFilename << '\n';
        }
    }

    Printer::~Printer () {
        if(TableLogFile.is_open()) {
            TableLogFile.close();
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

    void Printer::RecordState (RegisterFile* RF) {
        for (size_t i=0; i<32; i+=2) {
            RegisterFileRead RF_read = RF->Read(i, i+1);
            CurrentCycleSnapshot.register_data.push_back(RF_read.rs1);
            CurrentCycleSnapshot.register_data.push_back(RF_read.rs2);
        }
    }

    void Printer::EndCycle (int cycle_num) {
        CurrentCycleSnapshot.cycle_number = cycle_num;
        History.push_back(CurrentCycleSnapshot);

        CurrentCycleSnapshot = CycleSnapshot();
    }

    void Printer::PrintTable () {
        // Use tabulate datatypes here
        using namespace tabulate;

        size_t numCycle = History.size();

        Table table;
        Table::Row_t registerRow;
        registerRow.push_back("Registers");

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
                oss << "{ MemRead, MemWrite, SignExt, MemSize }: " << "{ " << out.mem_ctrl.MemRW << ", " << out.mem_ctrl.SignExt << ", " << out.mem_ctrl.MemSize << " }\n";
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
                oss << "{ RegWrite, MemtoReg }                 : " << "{ " << out.wb_ctrl.RegWrite << ", " << out.wb_ctrl.MemToReg << " }\n";
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

            Table registerTable;
            registerTable.add_row({"Register", "ABI Name", "Hex Value", "Decimal Value"});

            const std::string regNames[32] = {
                "x0",  "x1",  "x2",  "x3",  "x4",  "x5",
                "x6",  "x7",  "x8",  "x9",  "x10", "x11",
                "x12", "x13", "x14", "x15", "x16", "x17",
                "x18", "x19", "x20", "x21", "x22", "x23",
                "x24", "x25", "x26", "x27", "x28", "x29",
                "x30", "x31"
            };

            const std::string regName_ABI[32] = {
                "zero", "ra", "sp",  "gp",  "tp", "t0",
                "t1",   "t2", "s0",  "s1",  "a0", "a1",
                "a2",   "a3", "a4",  "a5",  "a6", "a7",
                "s2",   "s3", "s4",  "s5",  "s6", "s7",
                "s8",   "s9", "s10", "s11", "t3", "t4",
                "t5",   "t6"
            };

            // Add register data into internal table
            for (size_t i=0; i<snap.register_data.size(); i++) {
                registerTable.add_row( {regNames[i],
                                        regName_ABI[i],
                                        std::format("0x{:X}", snap.register_data[i]),
                                        std::format("{:d}", static_cast<int32_t>(snap.register_data[i]))}
                );
            }

            registerTable.format()
                         .font_style({FontStyle::bold})
                         .border_top("-")
                         .border_bottom("-")
                         .border_left("|")
                         .border_right("|")
                         .corner("+");

            registerTable.column(0).format()
                         .font_align(FontAlign::center);
            registerTable.column(1).format()
                         .font_align(FontAlign::center);
            registerTable.column(2).format()
                         .width(15)
                         .font_align(FontAlign::right);
            registerTable.column(3).format()
                         .width(20)
                         .font_align(FontAlign::right);

            // Add internal table to row
            registerRow.push_back(registerTable);
        }

        table.add_row(registerRow);

        table.format()
             .font_style({FontStyle::bold})
             .border_top("-")
             .border_bottom("-")
             .border_left("|")
             .border_right("|")
             .corner("+");

        table.row(0).format()
                    .font_align(FontAlign::center);

        table.column(0).format()
                    .font_align(FontAlign::center);

        table.row(6).format()
                    .font_align(FontAlign::center);

        // Make row header aligns vertically center
        for (size_t rowIdx = 1; rowIdx<table.size(); rowIdx++){

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
