#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <format>
#include <exception>

namespace RV32IM {

    class BaseError : public std::exception {
        public:
            std::string ExceptionName;
            std::string message;

            BaseError (const std::string& _name, const std::string& _message);
            const char* what() const noexcept override;
    };

    class ValueError : public BaseError {
        public:
            ValueError (const std::string& _message);
    };

    class MemoryError : public BaseError {
        public:
            MemoryError (const std::string& _message);
    };

    class SegmentationError : public BaseError {
        public:
            SegmentationError (const std::string& _message);
    };

    class PrintError : public BaseError {
        public:
            PrintError (const std::string& _message);
    };

    class ArgumentError : public BaseError {
        public:
            ArgumentError (const std::string& _message);
    };
}

#endif