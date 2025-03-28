#pragma once

namespace project {
    /**
     * @brief Enumeration of error codes used throughout the system
     */
    enum class ErrorCode {
        UNKNOWN = 0,          ///< Unknown or unspecified error
        VALIDATION_FAILED = 1000,  ///< Input validation failure
        RUNTIME_FAILED = 2000,     ///< Runtime operation failure
        SYSTEM_FAILED = 3000,      ///< System-level failure
        DATABASE_FAILED = 4000,    ///< Database operation failure
        NETWORK_FAILED = 5000      ///< Network operation failure
    };

    /**
     * @brief Categorization of different error types
     */
    enum class ErrorType {
        VALIDATION_ERROR,    ///< Input validation errors
        RUNTIME_ERROR,       ///< Runtime execution errors
        SYSTEM_ERROR,        ///< System-level errors
        EXTERNAL_ERROR,      ///< External service errors
        DATABASE_ERROR,      ///< Database-related errors
        NETWORK_ERROR        ///< Network-related errors
    };

    /**
     * @brief Severity levels for errors and logging
     */
    enum class ErrorSeverity {
        INFO,       ///< Informational messages
        WARNING,    ///< Warning messages
        ERROR,      ///< Error conditions
        CRITICAL    ///< Critical failures
    };

    /**
     * @brief Converts ErrorSeverity to its string representation
     * @param severity The severity level to convert
     * @return String representation of the severity level
     */
    inline const char* severity_to_string(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
}
