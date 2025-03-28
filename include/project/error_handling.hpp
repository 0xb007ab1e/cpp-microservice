#pragma once
#include "common/types.hpp"
#include "logging.hpp"
#include <stdexcept>
#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <vector>

namespace project {
    class ProjectError : public std::runtime_error {
    public:
        using Details = std::unordered_map<std::string, std::string>;
        using StackTrace = std::vector<std::string>;

        ProjectError(const std::string& message, 
                   ErrorType type,
                   ErrorCode code = ErrorCode::UNKNOWN,
                   ErrorSeverity severity = ErrorSeverity::ERROR,
                   Details details = {})
            : std::runtime_error(message)
            , type_(type)
            , code_(code)
            , severity_(severity)
            , details_(std::move(details)) {
                log_error(full_message());
            }
        
        ErrorType type() const { return type_; }
        ErrorCode code() const { return code_; }
        ErrorSeverity severity() const { return severity_; }
        const Details& details() const { return details_; }
        const StackTrace& stack_trace() const { return stack_trace_; }
        
        void add_stack_frame(const std::string& frame) {
            stack_trace_.push_back(frame);
        }
        
        std::string full_message() const {
            std::ostringstream oss;
            oss << "[" << severity_to_string(severity_) << "][Code: " 
                << static_cast<int>(code_) << "] " << what();
            
            if (!details_.empty()) {
                oss << " Details: {";
                for (const auto& [key, value] : details_) {
                    oss << " " << key << ": " << value << ";";
                }
                oss << " }";
            }

            if (!stack_trace_.empty()) {
                oss << "\nStack trace:\n";
                for (const auto& frame : stack_trace_) {
                    oss << "  " << frame << "\n";
                }
            }
            
            return oss.str();
        }

    private:
        ErrorType type_;
        ErrorCode code_;
        ErrorSeverity severity_;
        Details details_;
        StackTrace stack_trace_;
    };

    // Error factory functions
    inline ProjectError make_validation_error(
        const std::string& message,
        ProjectError::Details details = {}) {
        return ProjectError(message, 
                         ErrorType::VALIDATION_ERROR, 
                         ErrorCode::VALIDATION_FAILED,
                         ErrorSeverity::WARNING, 
                         std::move(details));
    }

    inline ProjectError make_runtime_error(
        const std::string& message,
        ProjectError::Details details = {}) {
        return ProjectError(message, 
                         ErrorType::RUNTIME_ERROR,
                         ErrorCode::RUNTIME_FAILED,
                         ErrorSeverity::ERROR, 
                         std::move(details));
    }

    inline ProjectError make_critical_error(
        const std::string& message,
        ErrorType type,
        ProjectError::Details details = {}) {
        ErrorCode code;
        switch (type) {
            case ErrorType::DATABASE_ERROR: 
                code = ErrorCode::DATABASE_FAILED; 
                break;
            case ErrorType::NETWORK_ERROR: 
                code = ErrorCode::NETWORK_FAILED; 
                break;
            case ErrorType::SYSTEM_ERROR: 
                code = ErrorCode::SYSTEM_FAILED; 
                break;
            default: 
                code = ErrorCode::UNKNOWN;
        }
        return ProjectError(message, type, code, ErrorSeverity::CRITICAL, std::move(details));
    }
}
