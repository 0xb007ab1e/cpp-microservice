#pragma once
#include "common/types.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>

namespace project {
    class Logger {
    public:
        static Logger& instance() {
            static Logger instance;
            return instance;
        }

        void log(const std::string& message, ErrorSeverity severity) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            
            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                << " [" << severity_to_string(severity) << "] "
                << message << std::endl;
            
            std::cout << oss.str();
            if (log_file_.is_open()) {
                log_file_ << oss.str();
                log_file_.flush();
            }
        }

        void set_log_file(const std::string& filename) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (log_file_.is_open()) {
                log_file_.close();
            }
            log_file_.open(filename, std::ios::app);
        }

    private:
        Logger() = default;
        ~Logger() {
            if (log_file_.is_open()) {
                log_file_.close();
            }
        }
        
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::ofstream log_file_;
        std::mutex mutex_;
    };

    // Convenience logging functions
    inline void log_info(const std::string& message) {
        Logger::instance().log(message, ErrorSeverity::INFO);
    }

    inline void log_warning(const std::string& message) {
        Logger::instance().log(message, ErrorSeverity::WARNING);
    }

    inline void log_error(const std::string& message) {
        Logger::instance().log(message, ErrorSeverity::ERROR);
    }

    inline void log_critical(const std::string& message) {
        Logger::instance().log(message, ErrorSeverity::CRITICAL);
    }
}
