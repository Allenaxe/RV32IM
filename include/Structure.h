#ifndef CPU_STRUCTURE
#define CPU_STRUCTURE
#include <iostream>
#include <cstdint>
#include <bitset>

namespace RV32IM {
    
    struct RegisterFileRead {
        uint32_t rs1;
        uint32_t rs2;
    };

    struct DecodeOutput {
        int32_t imm;
        uint32_t rs1;
        uint32_t rs2;
        std::bitset<5> rd;
        std::bitset<3> funct3;
        std::bitset<7> funct7;
        std::bitset<10> control_signal;
    };

    struct Printer {
        void operator() (const DecodeOutput& out) const {
            std::cout << "-----------------------------\n";
            std::cout << "[DECODE]\n";
            std::cout << "-----------------------------\n";
            std::cout << "rs1     : " << out.rs1 << '\n';
            std::cout << "rs2     : " << out.rs2 << '\n';
            std::cout << "rd      : " << out.rd << '\n';
            std::cout << "funct3  : " << out.funct3 << '\n';
            std::cout << "funct7  : " << out.funct7 << '\n';
            std::cout << "imm     : " << std::bitset<32>(out.imm) << "\n";
            std::cout << "-----------------------------\n";
        }

        // void operator()(const ExecuteOutput& out) const {
        //     std::cout << "[EXECUTE]\n";
        //     std::cout << "ALU op   : " << out.aluOp << '\n';
        //     std::cout << "rs1 data : " << out.rs1_data << '\n';
        //     std::cout << "rs2 data : " << out.rs2_data << '\n';
        //     std::cout << "ALU out  : " << out.alu_out << "\n";
        //     std::cout << "-----------------------------\n";
        // }

        // void operator()(const MemoryOutput& out) const {
        //     std::cout << "[MEMORY]\n";
        //     std::cout << "Address : " << out.mem_addr << '\n';
        //     std::cout << "Data    : " << out.mem_data << "\n";
        //     std::cout << "-----------------------------\n";
        // }

        // void operator()(const WritebackOutput& out) const {
        //     std::cout << "[WRITEBACK]\n";
        //     std::cout << "rd       : " << out.rd << '\n';
        //     std::cout << "WriteVal : " << out.writeback_value << "\n";
        //     std::cout << "-----------------------------\n";
        // }
    };
}

#endif