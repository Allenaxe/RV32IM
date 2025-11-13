#include "CPU.h"
#include "Memory.h"
#include "Loader.h"


int main() {
    RV32IM::Memory myMemory;

    RV32IM::Loader myLoader;
    std::unique_ptr<RV32IM::Segmentation> mySeg = myLoader.Load("machine_codes.txt", myMemory);
    std::cout << mySeg -> START_ADDR << " " << mySeg -> DATA_ADDR << " " << mySeg -> BSS_ADDR << " "
              << mySeg -> HEAP_ADDR << " " << mySeg -> END_ADDR << std::endl;

    
    


    // RV32IM::CPU* cpu = new RV32IM::CPU(myMemory);
    // cpu->Execute();
}