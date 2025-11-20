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

    struct Printer { 
        void operator() (const IF_ID_Data& out) const {
            std::cout << "-------------------------------------------\n";
            std::cout << "[FETCH]\n";
            std::cout << "-------------------------------------------\n";
            std::cout << "Program Counter: " << out.pc << '\n';
            std::cout << "Instruction    : " << out.inst << '\n';
            std::cout << "-------------------------------------------\n";
        }

        void operator() (const ID_EX_Data& out) const {
            std::cout << "-------------------------------------------\n";
            std::cout << "[DECODE]\n";
            std::cout << "-------------------------------------------\n";
            std::cout << "rs1            : " << out.rs1 << '\n';
            std::cout << "rs2            : " << out.rs2 << '\n';
            std::cout << "rd             : " << out.rd << '\n';
            std::cout << "funct3         : " << out.funct3 << '\n';
            std::cout << "funct7         : " << out.funct7 << '\n';
            std::cout << "imm            : " << std::bitset<32>(out.imm) << "\n";
            std::cout << "control signal : " << out.control_signal << "\n";
            std::cout << "-------------------------------------------\n";
        }

        void operator() (const EX_MEM_Data& out) const {
            std::cout << "-------------------------------------------\n";
            std::cout << "[EXECUTE]\n";
            std::cout << "-------------------------------------------\n";
            std::cout << "alu result     : " << out.alu_result << '\n';
            std::cout << "write data     : " << out.write_data << '\n';
            std::cout << "rd             : " << out.rd << '\n';
            std::cout << "control signal : " << out.control_signal << "\n";
            std::cout << "-------------------------------------------\n";
        }

        void operator() (const MEM_WB_Data& out) const {
            std::cout << "-------------------------------------------\n";
            std::cout << "[MEMORY]\n";
            std::cout << "-------------------------------------------\n";
            std::cout << "mem data       : " << out.mem_data << '\n';
            std::cout << "alu result     : " << out.alu_result << '\n';
            std::cout << "rd             : " << out.rd << '\n';
            std::cout << "control signal : " << out.control_signal << '\n';
            std::cout << "-------------------------------------------\n";
        }
    };
}

#endif