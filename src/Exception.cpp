#include "Exception.h"

namespace RV32IM {

    // BaseError
    BaseError::BaseError (const std::string& _name, const std::string& _message)
        : ExceptionName(_name), message(_message) {}

    const char* BaseError::what() const noexcept {
        static thread_local std::string formatted;
        formatted = std::format("{}: {}", ExceptionName, message);
        return formatted.c_str();
    }

    // ValueError
    ValueError::ValueError (const std::string& _message)
        : BaseError("ValeuError", _message) {};


    // MemoryError
    MemoryError::MemoryError (const std::string& _message)
        : BaseError("MemoryError", _message) {};

    // SegmentationError
    SegmentationError::SegmentationError (const std::string& _message)
        : BaseError("SegmentationError", _message) {};

    // PrintError
    PrintError::PrintError (const std::string& _message)
        : BaseError("PrintError", _message) {};
    
    // ArgumentError
    ArgumentError::ArgumentError (const std::string& _message)
        : BaseError("ArgumentError", _message) {};

}