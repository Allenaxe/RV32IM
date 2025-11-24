#include "CPU.h"
#include "Memory.h"
#include "Loader.h"
#include "Exception.h"

int main() {
    RV32IM::MainMemory myMemory;

    // RV32IM::Loader myLoader;
    // std::unique_ptr<RV32IM::Segmentation> mySeg = myLoader.Load("machine_codes.txt", myMemory);
    // std::cout << mySeg -> START_ADDR << " " << mySeg -> DATA_ADDR << " " << mySeg -> BSS_ADDR << " "
    //           << mySeg -> HEAP_ADDR << " " << mySeg -> END_ADDR << std::endl;

    // RV32IM::CPU* cpu = new RV32IM::CPU(mySeg, "Log.txt", true);
    // cpu->Run();


    try {
        throw RV32IM::ValueError("This value is reserved.");
    }
    catch (RV32IM::ValueError Exp){
        std::cout << Exp.ShowMessage() << std::endl;
    }
}