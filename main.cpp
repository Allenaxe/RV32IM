#include "CPU.h"
#include "Memory.h"

int main() {
    RV32IM::Memory* memory = new RV32IM::Memory();
    memory->Write(0, 0b00000000101000001000001010010011);
    RV32IM::CPU* cpu = new RV32IM::CPU(memory);
    cpu->Execute();
}