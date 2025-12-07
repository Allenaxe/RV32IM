# The Specification of CPU Simulator Based on RV32IM

This is a simulation CPU based on RV32IM.

Features:
- Highly consistent with the physical design.
- Pipeline structure is implemented.
- Provide runtime details.


## 1. Structure

To encapsule different kinds of data, we define lots of data types.
`Structure.h` may not so important at the first glance, but it does define some basic blocks of this project.

It contains:

- `RegisterFileRead` for reading register of register file.

- Enum classes that improve readability:
    - `MEM_RW` and `MEM_SIZE`: the structure of control signal of data memory
    - `ALU_OP_TYPE`: also a structure of signal, defining which operation that ALU need to perfome.

- Signals of each stage:
    - `ExecuteSignal`
    - `MemorySignal`
    - `WriteBackSignal`
    - `ControlSignal`: encapsule the previous three signals

- Data structures where pipeline registers storing data:
    - `IF_ID_DATA`
    - `ID_EX_Data`
    - `EX_MEM_Data`
    - `MEM_WB_Data`
    - `WB_Data`: Except this one. This is only for printer can store data more easily.

- `CycleSnapshot` for printer to store data of each stage in one clock.


## 2. Memory

The memory structure has two main components, main memory and segmentation.

### 2.1 Main Memory

Main memory is unique component in this project. It holds the storge for all instructions and data.
It have two constants which defining the range of memory we allocated:
- `PHY_LOW_ADDR`: the lowest allocated address (default as `0x0000'0000`)
- `PHY_HIGH_ADDR`: the highest allocated address (default as `0x000'5000`)

It also have a method `Clear()` which will clear all data in allocated memory.

### 2.2 Segmentation

`Segmentation` class inherits `MainMemory` class, which controls the segmentation of each program, so it can be more than one.

It might be weird for this CPU simulator, because it's one of the jobs that operating system handles. To better controls the valid memory area where a program can access, we finally decide to add segmentation into this project.

Segmentation has some properties and member functions:
``` C++
private:
    // Helper functions
    uint32_t AddrTranslate (const uint32_t p_Address);
    uint32_t EndianceToggle (uint32_t p_Data);

public:
    const uint32_t START_ADDR;      // Text section starts here
    const uint32_t DATA_ADDR;
    const uint32_t BSS_ADDR;
    const uint32_t HEAP_ADDR;
    const uint32_t END_ADDR;        // Stack section starts here

    Segmentation(uint32_t p_startAddr, uint32_t p_endAddr, uint32_t p_textLength);
    uint32_t Read (const uint32_t p_Address);
    void Write (const uint32_t& p_Address, const uint32_t& p_Value, std::bitset<4> p_ByteMask = std::bitset<4>("1111"));
```

It's similar to `MainMemory`, `Segmentation` has some constants which defined each segmentation, but they are **public** for program to use.
`START_ADDR`: the lowest address of segmentation, which is also the starting address of text section.
`DATA_ADDR`: the starting address of data section.
`BSS_ADDR`: the starting address of BSS section.
`HEAP_ADDR`: the starting address of heap section.
`END_ADDR`: the highest address of segmentation, which is also the starting address of stack section. Note that stack section will grow from high address to low address.

Constructor: Given the starting/ending address to allocated it on our main memory. The parameter `TextLength` denotes the instrution  length in bytes, which will make data section fit the length of program.<br>
`Read()`: Read the word at given memory address.<br>
`Write()`: Write the word at given memory address. It will decide which byte/half word or word to write. Fetch the word at that address, reconstruct data, and then write back to at address.


## 3. Instruction Memory

``` C++
class InstructionMemory {
    public:
        InstructionMemory ();
        uint32_t FetchInstr (std::unique_ptr<Segmentation>& p_Seg, uint32_t p_PC);
};
```

Instruction memory is responsible for fetching instructions for CPU, so it resides in instruction fetch (IF) stage.

It fetches instructions with the support of segmentation.


## 4. Control Unit

``` C++
class ControlUnit {
    public:
        static ControlSignal Generate(std::bitset<7> &p_Opcode, std::bitset<3> &funct3);
```

Control unit resides in instruction decode stage, generates control signal for components in the following stages.

The structure of control signal looks like this:
- `ControlSignal`
    - `ExecuteSignal`
        - `ALUSrc`: for selection of multiplexer in EX stage
        - `Branch`: indicate this command is branch instruction
        - `Jump`: indicate this command is jump instruction
        - `AUIPC`: indicate this command is AUIPC
        - `LUI`: indicate this command is LUI
    - `MemorySignal`
        - `MemRW`: instruct data memory to read/write
        - `SignExt`: tell data memory to do sign extension or not
        - `MemSize`: specify the size of data that data memory need to read/write
    - `WriteBackSignal`
        - `RegWrite`: write back data to register or not
        - `MemToReg`: select data from memory or from ALU result


## 5. Immediate Generator

``` C++
class ImmediateGenerator {
    private:
        static uint32_t DecodeType(std::bitset<7> p_Opcode);
        static uint32_t Extend(uint32_t p_Immediate, uint32_t p_Extend);

    public:
        static uint32_t Generate(uint32_t &p_Instr);
};
```

In RISC-V, we know that different types of instruction have different forms of immediate in instruction, so determine instruction type and generating immediates are the work of immediate generator.

Two helper functions `DecodeType()` and `Extend` do the same thing as its name looks like, and `Generate()` output immediate as the final result.


## 6. Register File

``` C++
class RegisterFile {
    private:
        uint32_t Register[NUMBER_REGISTER];

    public:
        RegisterFileRead Read(uint8_t rs1, uint8_t rs2);
        void Write(uint8_t rd, uint32_t p_WriteData, bool p_WriteEnable);
};
```

Register file provide the interface between read from and write to  register.<br>
`Read()`: It reads both `rs1` and `rs2` at one time.<br>
`Write()`: If singal `WriteEnable` is true, write `WriteData` to register `rd`.


## 7. Arithmetic Logic Unit (ALU)

``` C++
class ALU {
    public:
        static int32_t Generate_OpA (const uint32_t PC, uint32_t p_Src1, bool p_Selector);
        static int32_t Generate_OpB (uint32_t p_Src2, const int32_t imm, bool p_Selector);
        static int32_t Operate (std::bitset<4> p_ALUFunct, ALU_OP_TYPE p_ALUOp, int32_t p_OpA, int32_t p_OpB);
        // AluControl ( 4 bits ) - 3 bits funct3 & 1 bit funct7
        static std::bitset<4> AluControl (uint32_t p_funct3, uint32_t p_funct7);
    private:
        static int32_t Adder (uint32_t p_Src1, uint32_t p_Src2, bool cin, bool &carry);
};
```

To simplify the scenario, we put forwarding aside first.

The ALU accept two parameters to do arithmetic, which is `Op_A` and `Op_B`.
In order to know which calculation to perform, ALU also need two signals:
- `p_ALUOp`: It generated from previous stage, emitted by control unit. It carries the information about the type of instructions, like I-type, R-type, etc.
- `p_ALUFunct`: It generated from ALU Control and tells ALU to do what specific operation under given type.

The core of ALU is adder. We implement this in the full-adder way, so it will do bit operations at each bit and return the final result.

If we want to do pipelining and try to reach best performance, the forwarding unit is necessary. <br>
We have two multiplexers. They are doing the same thing, which is identify the `rs` (both `rs1` and `rs2`, that's why we have two multiplexers) of current instruction as same as `rd` from previous **two** instructions. As for which `rd` will be forwarded, from execution stage or memory stage, it will be determined by the signal `RegWrite` of those instructions.


## 8. ALU Control

As mentioned before, ALU control generates `p_ALUFunct`. It determine the unique arithmetic operation that ALU will perform by `funct3` and `funct7` which specified by RISC-V to indicating acutal ALU operations.


## 9. Load Store Unit (LSU)

It's the component which determine the address of memory accessing is aligned/unaligned, and also returns a byte mask which indicates which bytes in a word to read from or write to memory.

``` C++
public:
    LoadStoreUnit();

    // Generate a tuple (AlignedAddr, ByteMask)
    std::tuple<uint32_t, std::bitset<4>> DecodeAddr(uint32_t p_Addr, MEM_SIZE MemSize);
```

`DecodeAddr()`: Determine address alignment by given address and the size of memory accessing (ex: byte / half word / word). It also returns a 4-byte-aligned address and byte mask indicting which bytes in a word to access for future use (mainly in data memory).


## 10. Data Memory


``` C++
private:
    int32_t imm;
    uint32_t addr;

    // Helper function
    size_t GetTrailingZero (std::bitset<4> p_ByteMask);

    uint32_t Load (int32_t p_Addr, std::bitset<4> p_ByteMask, bool SignExt);
    std::nullopt_t Store (uint32_t p_AlignedAddr, std::bitset<4> p_ByteMask, int32_t p_Imm);

public:
    DataMemory (std::unique_ptr<Segmentation>& p_Seg);
    std::unique_ptr<Segmentation> seg;
    std::optional<uint32_t> Operate (MemRW_t MemRW, bool SignExt, std::bitset<4> ByteMask, uint32_t p_AlignedAddr, int32_t p_Imm);
```

Constructor: Accept a unqiue_ptr of segmentation for memory accessing.

`DataMemory` works while `Operate()` being called. It will determine calling load/store by `MemRW`:
- `MemRW` is `MemRW_t::NOP`: return `std::nullopt`
- `MemRW` is `MemRW_t::READ`: call `Load()`
- `MemRW` is `MemRW_t::WIRE`: call `Store()`

`Load()`: Load the specific part of memory.
- `p_Aligned4_Addr` is the 4-byte-aligned address.
- `p_ByteMask`: controls which byte(s) should be read.
- `SignExt` tell `DataMemory` to do signed extension or not.

`Store()`: Store the data to memory.
- `p_Aligned4_Addr`: the 4-byte-aligned address.
- `p_ByteMask`: controls which byte(s) should be read.
- `p_Imm`: the data to store.

And helper function:
`GetTrailingZero()`: Calculate the trailing zeros of byte mask.

## 11. File structure and UML Diagrams

File structure shown below
```
├── cpu_simulator
├── include
│   ├── ALU.h
│   ├── Component.h
│   ├── CPU.h
│   ├── DataMemory.h
│   ├── Exception.h
│   ├── ForwardingUnit.h
│   ├── InstructionMemory.h
│   ├── Loader.h
│   ├── LoadStoreUnit.h
│   ├── Memory.h
│   ├── PipelineRegister.h
│   ├── Printer.h
│   └── Structure.h
├── LICENSE
├── Log.txt
├── machine_codes.txt
├── Makefile
├── README.md
└── src
    ├── ALU.cpp
    ├── Component.cpp
    ├── CPU.cpp
    ├── DataMemory.cpp
    ├── Exception.cpp
    ├── ForwardingUnit.cpp
    ├── InstructionMemory.cpp
    ├── Loader.cpp
    ├── LoadStoreUnit.cpp
    ├── Log.txt
    ├── main.cpp
    ├── Memory.cpp
    └── Printer.cpp
```

And we put the all diagrams (datapath and UML) in the folder of `docs/`, including  a complete diagram and separated diagrams for each class.


## 12. Future Work

Since this was a final project for a class, we tried our best to make the current results within a month. Some features that haven't been implemented yet may make it look less like a true RISC-V CPU.

Missing features including but not limit to:
- Flexibility of address defined in segmentation
- Hardware trap

## 13. Dependencies and Tools

Some open projects faciliate our work, and they are worth to be mentioned here!

- [**drawio**](https://github.com/jgraph/drawio): For drawing digram of datapath.
- [**clang-uml**](https://github.com/bkryza/clang-uml): For generating UML graph.


## 14. Contribution

- doctorxsx <allen31042@gmail.com>
- smolsquirrel936 <smolsquirrel936@gmail.com>
- yus091 <ysheng910316@gmail.com>