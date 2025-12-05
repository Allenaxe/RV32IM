#include "Exception.h"

namespace RV32IM {

    // BaseError
    BaseError::BaseError (std::string _message) {
        message = _message;
    }

    std::string BaseError::ShowMessage(){
        // return std::format("{}: {}", ExceptionName, message);
        return ExceptionName;
    }

    // ValueError
    ValueError::ValueError (std::string _message) : BaseError(_message) {};


    // MemoryError
    MemoryError::MemoryError (std::string _message) : BaseError(_message) {};

    // SegmentationError
    SegmentationError::SegmentationError (std::string _message) : BaseError(_message) {};
}