#include "CPU.h"
#include "ForwardingUnit.h"
#include "ALU.h"

namespace RV32IM {

    CPU::CPU (std::unique_ptr<Segmentation>& p_ProgSeg,
              std::string TableFilename)
        : RF(new RegisterFile()),
          DM(new DataMemory(p_ProgSeg)),
          Record(new Printer(TableFilename))
    {
        PC = DM->seg->START_ADDR;
        IF_ID = {};
        ID_EX = {};
        EX_MEM = {};
        MEM_WB = {};
    }

    IF_ID_Data CPU::Fetch () {
        MAR = PC;
        MDR = InstrMem.FetchInstr(DM->seg, MAR);
        IR = MDR;

        PC += 4;

        std::cout << PC << std::endl;
        std::cout << std::hex << IR << std::endl;
        // std::cin.get();

        return IF_ID_Data {
            PC,
            IR
        };
    }

    ID_EX_Data CPU::Decode (IF_ID_Data& p_DecodeInput) {
        std::bitset<7> opcode       { p_DecodeInput.inst & 0x0000'007F};           // IR[0:6]
        std::bitset<5> rd           {(p_DecodeInput.inst & 0x0000'0F80) >> 7};     // IR[7:11]
        std::bitset<3> funct3       {(p_DecodeInput.inst & 0x0000'7000) >> 12};    // IR[12:14]
        uint8_t rs1 =               ( p_DecodeInput.inst & 0x000F'8000) >> 15;     // IR[15:19]
        uint8_t rs2 =               ( p_DecodeInput.inst & 0x01F0'0000) >> 20;     // IR[20:24]
        std::bitset<7> funct7       {(p_DecodeInput.inst & 0xFE00'0000) >> 25};    // IR[25:31]

        uint32_t imm = ImmediateGenerator::Generate(p_DecodeInput.inst);
        ControlSignal control_signal = ControlUnit::Generate(opcode, funct3, funct7);

        RegisterFileRead RF_read = RF->Read(rs1, rs2);

        // Branch decision (for JAL)
        if (control_signal.ex_ctrl.Jump) {
            PC = p_DecodeInput.pc + imm;
            IF_ID.SetClear();
        }

        return ID_EX_Data {
            RF_read.rs1,
            RF_read.rs2,
            imm,
            rd,
            rs1,
            rs2,
            funct3,
            funct7,
            control_signal.ex_ctrl,
            control_signal.mem_ctrl,
            control_signal.wb_ctrl
        };
    }

    EX_MEM_Data CPU::Execute (ID_EX_Data& p_ExecuteInput) {
        uint32_t rs1 = ForwardingUnit::ALUMux(p_ExecuteInput.RS1, p_ExecuteInput.rs1, EX_MEM.Read(), MEM_WB.Read());
        uint32_t rs2 = ForwardingUnit::ALUMux(p_ExecuteInput.RS2, p_ExecuteInput.rs2, EX_MEM.Read(), MEM_WB.Read());
        int32_t opA = ALU::Generate_OpA(PC, rs1, (p_ExecuteInput.ex_ctrl.Branch | p_ExecuteInput.ex_ctrl.Jump | p_ExecuteInput.ex_ctrl.AUIPC));
        int32_t opB = ALU::Generate_OpB(rs2, p_ExecuteInput.imm, p_ExecuteInput.ex_ctrl.ALUSrc);
        std::bitset<4> aluControl = ALU::AluControl(p_ExecuteInput.funct3.to_ulong(), p_ExecuteInput.funct7.to_ulong());
        ALU_OP_TYPE control_signal = p_ExecuteInput.ex_ctrl.ALUOp;

        std::tuple<uint32_t, bool> alu_output = ALU::Operate(aluControl, control_signal, opA, opB);
        uint32_t alu_result = std::get<0>(alu_output);
        bool zeroFlag = std::get<1>(alu_output);

        // Branch decision (for JALR and B-type)
        if (p_ExecuteInput.ex_ctrl.Branch && zeroFlag) {
            PC = alu_result;
            IF_ID.SetClear();
            ID_EX.SetClear();
        }

        return EX_MEM_Data {
            static_cast<uint32_t>(alu_result),
            p_ExecuteInput.rs2,
            p_ExecuteInput.rd,
            p_ExecuteInput.mem_ctrl,
            p_ExecuteInput.wb_ctrl
        };
    }

    MEM_WB_Data CPU::Memory (EX_MEM_Data& p_MemoryInput) {
        MEM_RW MemRW = p_MemoryInput.mem_ctrl.MemRW;
        bool SignExt = p_MemoryInput.mem_ctrl.SignExt;
        MEM_SIZE MemSize = p_MemoryInput.mem_ctrl.MemSize;

        std::tuple<uint32_t, std::bitset<4>> AddrPack = LoadStoreUnit::DecodeAddr(p_MemoryInput.alu_result, MemSize);

        std::optional<uint32_t> mem_data = DM->Operate(MemRW, SignExt, std::get<1>(AddrPack), std::get<0>(AddrPack), p_MemoryInput.write_data);

        return MEM_WB_Data {
            mem_data,
            p_MemoryInput.alu_result,
            p_MemoryInput.rd,
            p_MemoryInput.wb_ctrl
        };
    }

    WB_Data CPU::WriteBack(MEM_WB_Data& p_WriteBackInput) {
        // MemtoReg: select data from memory or ALU result
        uint32_t writeback_data =
            p_WriteBackInput.wb_ctrl.MemToReg ? p_WriteBackInput.mem_data.value_or(0) : p_WriteBackInput.alu_result;

        // RegWrite: write back to Register File
        if(p_WriteBackInput.wb_ctrl.RegWrite)
            RF->Write(p_WriteBackInput.rd.to_ulong(), writeback_data, p_WriteBackInput.wb_ctrl.RegWrite);

        return WB_Data { writeback_data, p_WriteBackInput.rd, p_WriteBackInput.wb_ctrl.Halt };
    }

    void CPU::Run () {

        // While instruction is not empty, run
        int cycle = 1;

        // Main loop for execution
        while (true) {

            // Write-Back (WB) Stage
            MEM_WB_Data writeback_input = MEM_WB.Read();
            WB_Data writeback_output = WriteBack(writeback_input);
            Record->RecordState(writeback_output);

            // Memory (MEM) Stage
            EX_MEM_Data memory_input = EX_MEM.Read();
            MEM_WB_Data memory_output = Memory(memory_input);
            Record->RecordState(memory_output);
            MEM_WB.Write(memory_output);

            // Execute (EX) Stage
            ID_EX_Data execute_input = ID_EX.Read();
            EX_MEM_Data execute_output = Execute(execute_input);
            Record->RecordState(execute_output);
            EX_MEM.Write(execute_output);

            // Decode (ID) Stage
            IF_ID_Data decode_input = IF_ID.Read();
            ID_EX_Data decode_output = Decode(decode_input);
            Record->RecordState(decode_output);
            ID_EX.Write(decode_output);

            // Fetch (IF) Stage
            IF_ID_Data fetch_output = Fetch();
            Record->RecordState(fetch_output);
            IF_ID.Write(fetch_output);

            Record->RecordState(RF);
            Record->RecordClearSignals(IF_ID.GetClear(), ID_EX.GetClear());

            std::cout << "IF_ID Clear: " << IF_ID.GetClear() << std::endl;
            std::cout << "ID_EX Clear: " << ID_EX.GetClear() << std::endl;

            IF_ID.Update();
            ID_EX.Update();
            EX_MEM.Update();
            MEM_WB.Update();

            Record->EndCycle(cycle);
            ++cycle;

            if(writeback_output.Halt) break;
        }

        // After Loop
        Record->PrintTable();
    }
}