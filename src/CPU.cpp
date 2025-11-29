#include "CPU.h"

namespace RV32IM {
    CPU::CPU(std::unique_ptr<Segmentation>& p_ProgSeg, std::string Filename, bool ConsoleOutput): RF(new RegisterFile()), ProgSeg(p_ProgSeg), Record(new Printer(Filename, ConsoleOutput)) {
        PC = ProgSeg->START_ADDR;
        IF_ID = {};
        ID_EX = {};
        EX_MEM = {};
        MEM_WB = {};
    }
    void CPU::Fetch() {
        MAR = PC;
        MDR = ProgSeg->Read(MAR);
        IR = MDR;
        PC += 4;
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

        return ID_EX_Data { RF_read.rs1, RF_read.rs2, imm, rd, rs1, rs2, funct3, funct7, control_signal };
    }

    EX_MEM_Data CPU::Execute(ID_EX_Data& p_ExecuteInput) {
        int32_t opA = ALU::OpA(PC, p_ExecuteInput.rs1, (p_ExecuteInput.control_signal[4] | p_ExecuteInput.control_signal[5]));
        int32_t opB = ALU::OpB(p_ExecuteInput.rs2, p_ExecuteInput.imm, p_ExecuteInput.control_signal[8]);
        opA = ForwardingUnit::ALUMux(p_ExecuteInput.RS1, opA, EX_MEM.Read(), MEM_WB.Read());
        opB = ForwardingUnit::ALUMux(p_ExecuteInput.RS2, opB, EX_MEM.Read(), MEM_WB.Read());

        std::bitset<4> aluControl = ALU::AluControl(p_ExecuteInput.funct3.to_ulong(), p_ExecuteInput.funct7.to_ulong());
        int32_t control_signal = p_ExecuteInput.control_signal.to_ulong() & 0b111;
        int32_t alu_output = ALU::Operate(aluControl, control_signal, opA, opB);
        
        return EX_MEM_Data {static_cast<uint32_t>(alu_output), p_ExecuteInput.rs2, p_ExecuteInput.rd, p_ExecuteInput.control_signal};
    }

    MEM_WB_Data CPU::Memory(EX_MEM_Data& p_MemoryInput) {
        return MEM_WB_Data {};
    }

    WB_Data CPU::WriteBack(MEM_WB_Data& p_WriteBackInput) {
        uint32_t writeback_data = p_WriteBackInput.control_signal[3] ? // MemtoReg: select data from memory or ALU result
            p_WriteBackInput.mem_data : p_WriteBackInput.alu_result;

        if(p_WriteBackInput.control_signal[9]) // RegWrite: write back to Register File
            RF->Write(p_WriteBackInput.rd.to_ulong(), writeback_data, p_WriteBackInput.control_signal[9]);

        return WB_Data {writeback_data, p_WriteBackInput.rd, p_WriteBackInput.control_signal};
    }

    void CPU::Run() {

        // while instruction is not empty, run
        int cycle = 1;

        while (true) {

            // Terminate Condition
            if (cycle == 6) break;

            // ---------------------------------------------
            // Fetch (IF) Stage
            // ---------------------------------------------
            Fetch();
            IF_ID_Data fetch_output {PC, IR};
            Record->RecordState(fetch_output);
            IF_ID.Write(fetch_output);

            // ---------------------------------------------
            // Decode (ID) Stage
            // ---------------------------------------------
            IF_ID_Data decode_input = IF_ID.Read();
            ID_EX_Data decode_output = Decode(decode_input);
            Record->RecordState(decode_output);
            ID_EX.Write(decode_output);

            // ---------------------------------------------
            // Execute (EX) Stage
            // ---------------------------------------------
            ID_EX_Data execute_input = ID_EX.Read();
            EX_MEM_Data execute_output = Execute(execute_input);
            Record->RecordState(execute_output);
            EX_MEM.Write(execute_output);

            // ---------------------------------------------
            // Memory (MEM) Stage
            // ---------------------------------------------
            EX_MEM_Data memory_input = EX_MEM.Read();
            MEM_WB_Data memory_output = Memory(memory_input);
            Record->RecordState(memory_output);
            MEM_WB.Write(memory_output);

            // ---------------------------------------------
            // Write-Back (WB) Stage
            // ---------------------------------------------
            MEM_WB_Data writeback_input = MEM_WB.Read();
            WB_Data writeback_output = WriteBack(writeback_input);
            Record->RecordState(writeback_output);

            IF_ID.Update();
            ID_EX.Update();
            EX_MEM.Update();
            MEM_WB.Update();

            Record->EndCycle(cycle);
            ++cycle;
        }

        // After Loop
        Record->PrintTrace();
    }
}