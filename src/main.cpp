#include "CPU.h"
#include "Memory.h"
#include "Loader.h"
#include "Exception.h"

int main(int argc, char* argv[]) {

    std::string machineCodeFile = "machine_codes.txt";
    std::string table_filename  = "Log_table.txt";

    // Argument parser
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-m" && i + 1 < argc) {
            machineCodeFile = argv[++i];
        } else if (arg == "-l" && i + 1 < argc) {
            table_filename = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Usage: " << argv[0] << " [-m machine_code_file] [-l table_logfile] [--no-console]\n";
            return 1;
        }
    }

    RV32IM::MainMemory myMemory;

    RV32IM::Loader myLoader;
    std::unique_ptr<RV32IM::Segmentation> mySeg = myLoader.Load(machineCodeFile, myMemory);
    std::cout << mySeg -> START_ADDR << " " << mySeg -> DATA_ADDR << " " << mySeg -> BSS_ADDR << " "
              << mySeg -> HEAP_ADDR << " " << mySeg -> END_ADDR << std::endl;

    RV32IM::CPU* cpu = new RV32IM::CPU(mySeg, table_filename);
    cpu->Run();

}