#include "CPU.h"

namespace RV32IM {
    CPU::CPU(MainMemory* p_TheMemory): RF(new RegisterFile()), TheMemory(p_TheMemory), Record(new Utility()) {}
    void CPU::Fetch() {
        MAR = PC;
        MDR = TheMemory->Read(MAR);
        IR = MDR;
        PC += 1;
    }

    ID_EX_Data CPU::Decode(IF_ID_Data& p_DecodeInput) { 
        std::bitset<7> opcode {p_DecodeInput.inst & 0x0000'007F};            // IR[0:6]
        std::bitset<5> rd {(p_DecodeInput.inst & 0x0000'0F80) >> 7};         // IR[7:11]
        std::bitset<3> funct3 {(p_DecodeInput.inst & 0x0000'7000) >> 12};    // IR[12:14]
        uint8_t rs1 = (p_DecodeInput.inst & 0x000F'8000) >> 15;              // IR[15:19]
        uint8_t rs2 = (p_DecodeInput.inst & 0x01F0'0000) >> 20;              // IR[20:24]
        std::bitset<7> funct7 {(p_DecodeInput.inst & 0xFE00'0000) >> 25};    // IR[25:31]

        uint32_t imm = ImmediateGenerator::Generate(p_DecodeInput.inst);
        std::bitset<10> control_signal = ControlUnit::ControlSignal(opcode);

        RegisterFileRead RF_read = RF->Read(rs1, rs2);

        return ID_EX_Data { RF_read.rs1, RF_read.rs2, imm, rd, funct3, funct7, control_signal };
    }

    EX_MEM_Data CPU::Execute(ID_EX_Data& p_ExecuteInput) {
        return EX_MEM_Data {};
    }

    MEM_WB_Data CPU::Memory(EX_MEM_Data& p_MemoryInput) {
        return MEM_WB_Data {};
    }

    void CPU::WriteBack(MEM_WB_Data& p_WriteBackInput) {

    }

    void CPU::Run() {

        // while instruction is not empty, run
        int cycle = 0;

        while (true) {

            // Terminate Condition
            if (cycle == 5) break;

            // ---------------------------------------------
            // Write-Back (WB) Stage
            // ---------------------------------------------
            MEM_WB_Data writeback_input = MEM_WB.Read();
            WriteBack(writeback_input);

            // ---------------------------------------------
            // Memory (MEM) Stage
            // ---------------------------------------------
            EX_MEM_Data memory_input = EX_MEM.Read();
            MEM_WB_Data memory_output = Memory(memory_input);
            Record->Record(memory_output);
            MEM_WB.Write(memory_output);

            // ---------------------------------------------
            // Execute (EX) Stage
            // ---------------------------------------------
            ID_EX_Data execute_input = ID_EX.Read();
            EX_MEM_Data execute_output = Execute(execute_input);
            Record->Record(execute_output);
            EX_MEM.Write(execute_output);
            
            // ---------------------------------------------
            // Decode (ID) Stage
            // ---------------------------------------------
            IF_ID_Data decode_input = IF_ID.Read();
            ID_EX_Data decode_output = Decode(decode_input);
            Record->Record(decode_output);
            ID_EX.Write(decode_output);
            
            // ---------------------------------------------
            // Fetch (IF) Stage
            // ---------------------------------------------
            Fetch();
            IF_ID_Data fetch_output {PC, IR};
            Record->Record(fetch_output);
            IF_ID.Write(fetch_output);

            IF_ID.Update();
            ID_EX.Update();
            EX_MEM.Update();
            MEM_WB.Update();

            ++cycle;
        }

        // After Loop
        Record->Print();
    }
}