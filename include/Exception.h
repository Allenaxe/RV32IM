#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>
// #include <format>

namespace RV32IM {

    class BaseError {
        public:
            const std::string ExceptionName;
            std::string message;

            BaseError (std::string _message);
            std::string ShowMessage ();
    };

    class ValueError : public BaseError {
        public:
            const std::string ExceptionName = "ValeuError";
            ValueError (std::string _message);
    };

    class MemoryError : public BaseError {
        public:
            const std::string ExceptionName = "MemoryError";
            MemoryError (std::string _message);
    };

    class SegmentationError : public BaseError {
        public:
            const std::string ExceptionName = "SegmentationError";
            SegmentationError (std::string _message);
    };

}

#endif