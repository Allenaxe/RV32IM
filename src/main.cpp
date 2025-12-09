#include "CPU.h"
#include "Memory.h"
#include "Loader.h"
#include "Exception.h"

#include <filesystem>

int main() {

    // std::string machineCodeFile = "machine_codes.txt";
    // std::string table_filename  = "Log_table.txt";

    // // Argument parser
    // for (int i = 1; i < argc; ++i) {
    //     std::string arg = argv[i];
    //     if (arg == "-m" && i + 1 < argc) {
    //         machineCodeFile = argv[++i];
    //     } else if (arg == "-l" && i + 1 < argc) {
    //         table_filename = argv[++i];
    //     } else {
    //         std::cerr << "Unknown argument: " << arg << std::endl;
    //         std::cerr << "Usage: " << argv[0] << " [-m machine_code_file] [-l table_logfile] [--no-console]\n";
    //         return 1;
    //     }
    // }

    std::filesystem::path test_root = "testcase";
    std::filesystem::path result_root = "result";

    RV32IM::MainMemory myMemory;

    RV32IM::Loader myLoader;

    for (const auto& entry : std::filesystem::directory_iterator(test_root)) {
        if (!entry.is_directory())
            continue;

        std::filesystem::path testcase = entry.path();
        for (const auto& fileEntry : std::filesystem::directory_iterator(testcase)) {
            if(!fileEntry.is_regular_file())
                continue;
                
            std::filesystem::path machineCodeFile = fileEntry.path();
            std::unique_ptr<RV32IM::Segmentation> mySeg = myLoader.Load(machineCodeFile, myMemory);
            std::cout << mySeg -> START_ADDR << " " << mySeg -> DATA_ADDR << " " << mySeg -> BSS_ADDR << " "
                    << mySeg -> HEAP_ADDR << " " << mySeg -> END_ADDR << std::endl;

            std::filesystem::path relativePath = std::filesystem::relative(testcase, test_root);
            
            std::filesystem::path result = result_root / relativePath;

            if (!std::filesystem::exists(result))
                std::filesystem::create_directories(result);

            result = result / machineCodeFile.filename();
            
            RV32IM::CPU* cpu = new RV32IM::CPU(mySeg, result.string());
            cpu->Run();
        }
    }

}