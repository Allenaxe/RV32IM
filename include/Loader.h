#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <bitset>
#include <vector>
#include <memory>

#include "Memory.h"

namespace RV32IM{

    class Loader{
        private:
            uint32_t startAddr;

        public:
            Loader();
            std::unique_ptr<Segmentation> Load(std::string p_filename);
    };

}

#endif