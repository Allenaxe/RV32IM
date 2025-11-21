#include "CPU.h"
#include "Memory.h"

int main() {
    RV32IM::Memory* memory = new RV32IM::Memory();
    memory->Write(0, 0b0000'0000'1010'0000'1000'0010'1001'0011);
    RV32IM::CPU* cpu = new RV32IM::CPU(memory);
    cpu->Execute();
    cpu->Print();
}