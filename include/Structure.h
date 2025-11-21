#ifndef CPU_STRUCTURE
#define CPU_STRUCTURE
#include <iostream>
#include <optional>
#include <cstdint>
#include <bitset>

namespace RV32IM {
    
    struct RegisterFileRead {
        uint32_t rs1;
        uint32_t rs2;
    };

    struct ControlSignal {
        bool RegWrite;
		bool ALUSrc;
		bool MemRead;
		bool MemWrite;
		bool Branch;
		bool Jump;
		bool MemtoReg;
		bool ALUOp[3];
    };

    // IF/ID Register
    struct IF_ID_Data {
        uint32_t pc;
        uint32_t inst;
    };

    // ID/EX Register
    struct ID_EX_Data {
        uint32_t rs1;
        uint32_t rs2;
        uint32_t imm;
        std::bitset<5> rd;
        std::bitset<3> funct3;
        std::bitset<7> funct7;
        std::bitset<10> control_signal;
        // ControlSignals ctrl;
    };

    // EX/MEM Register
    struct EX_MEM_Data {
        uint32_t alu_result;
        uint32_t write_data;
        std::bitset<5>  rd;
        std::bitset<10> control_signal;
        // ControlSignals ctrl;
    };

    // MEM/WB Register
    struct MEM_WB_Data {
        uint32_t mem_data;
        uint32_t alu_result;
        std::bitset<5> rd;
        std::bitset<10> control_signal;
        // ControlSignals ctrl;
    };

     struct WB_Data {
        uint32_t writeback_data;
        std::bitset<5> rd;
        std::bitset<10> control_signal;
        // ControlSignals ctrl;
    };

    struct CycleSnapshot {
        int cycle_number;
        std::optional<IF_ID_Data>  IF_data;
        std::optional<ID_EX_Data>  ID_data;
        std::optional<EX_MEM_Data> EX_data;
        std::optional<MEM_WB_Data> MEM_data;
        std::optional<WB_Data> WB_data;
    };
}

#endif