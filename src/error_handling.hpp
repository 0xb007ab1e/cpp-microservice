#pragma once
#include <stdexcept>
#include <string>

namespace project {
    enum class ErrorType {
        VALIDATION_ERROR,
        RUNTIME_ERROR,
        SYSTEM_ERROR,
        EXTERNAL_ERROR
    };

    class ProjectError : public std::runtime_error {
    public:
        ProjectError(const std::string& message, ErrorType type)
            : std::runtime_error(message), type_(type) {}
        
        ErrorType type() const { return type_; }
    private:
        ErrorType type_;
    };
}
