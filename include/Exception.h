#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

namespace RV32IM {

    class BaseError {
        public:
            std::string message;

            BaseError(std::string _message);
            std::string ShowMessage ();
    };

    class ValueError : public BaseError {
        public:
            ValueError(std::string _message);
    };

    class MemoryError : public BaseError {
        public:
            MemoryError(std::string _message);
    };
}

#endif