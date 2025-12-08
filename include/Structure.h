#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <cstdint>
#include <bitset>
#include <optional>

namespace RV32IM {

    struct RegisterFileRead {
        uint32_t rs1;
        uint32_t rs2;
    };

    enum class MEM_RW : uint8_t {
        NOP   = 0,              // Do nothing
        READ  = 1,              // Able to read
        WRITE = 2,              // Able to write
    };

    enum class MEM_SIZE : uint8_t {
        BYTE = 0,               // Corresponding to funct3: 000 (LB/SB)
        HALF = 1,               // Corresponding to funct3: 001 (LH/SH)
        WORD = 2                // Corresponding to funct3: 010 (LW/SW)
    };

    // Types of ALU operation (the signal which decoder sending to ALU)
    enum class ALU_OP_TYPE : uint8_t {
        MEMORY_REF = 0,         // Load/Store (Bit: 000)
        BRANCH     = 1,         // Branch     (Bit: 001)
        R_TYPE     = 2,         // R-Type     (Bit: 010)
        I_TYPE     = 3,         // I-Type     (Bit: 011)
        LUI        = 4,         // LUI        (Bit: 100)
        AUIPC      = 5          // AUIPC      (Bit: 101)
    };

    struct ExecuteSignal {
        bool ALUSrc;
        bool Branch;
        bool Jump;
        bool AUIPC;
        bool LUI;
        ALU_OP_TYPE ALUOp;
    };

    struct MemorySignal {
        MEM_RW MemRW;
        bool SignExt;
        MEM_SIZE MemSize;
    };

    struct WriteBackSignal {
        bool RegWrite;
        bool MemToReg;
        bool Halt;
    };

    struct ControlSignal {
        ExecuteSignal ex_ctrl;
        MemorySignal mem_ctrl;
        WriteBackSignal wb_ctrl;
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
        std::bitset<5> RS1;
        std::bitset<5> RS2;
        std::bitset<3> funct3;
        std::bitset<7> funct7;
        ExecuteSignal ex_ctrl;
        MemorySignal mem_ctrl;
        WriteBackSignal wb_ctrl;
    };

    // EX/MEM Register
    struct EX_MEM_Data {
        uint32_t alu_result;
        uint32_t write_data;
        std::bitset<5>  rd;
        MemorySignal mem_ctrl;
        WriteBackSignal wb_ctrl;
    };

    // MEM/WB Register
    struct MEM_WB_Data {
        std::optional<uint32_t> mem_data;
        uint32_t alu_result;
        std::bitset<5> rd;
        WriteBackSignal wb_ctrl;
    };

     struct WB_Data {
        uint32_t writeback_data;
        std::bitset<5> rd;
        bool Halt;              // Indicate whether to halt the CPU
    };

    struct CycleSnapshot {
        int cycle_number;
        std::optional<IF_ID_Data>  IF_data;
        std::optional<ID_EX_Data>  ID_data;
        std::optional<EX_MEM_Data> EX_data;
        std::optional<MEM_WB_Data> MEM_data;
        std::optional<WB_Data>     WB_data;
    };

}

#endif