#pragma once
#include <string>
#include <unordered_map>
#include <chrono>

namespace project {
    enum class ServiceState {
        CREATED,
        INITIALIZED,
        RUNNING,
        STOPPED,
        ERROR
    };

    class Status {
    public:
        Status(bool ok = true, const std::string& message = "")
            : ok_(ok), message_(message) {}
        
        bool isOk() const { return ok_; }
        const std::string& message() const { return message_; }
        
    private:
        bool ok_;
        std::string message_;
    };

    class HealthStatus {
    public:
        HealthStatus(bool healthy, const std::string& status)
            : healthy_(healthy), status_(status) {}
        
        bool isHealthy() const { return healthy_; }
        const std::string& getStatus() const { return status_; }
        
    private:
        bool healthy_;
        std::string status_;
    };

    using ServiceConfig = std::unordered_map<std::string, std::string>;
    using ServiceMetrics = std::unordered_map<std::string, std::string>;

    class BaseService {
    public:
        explicit BaseService(const std::string& name) 
            : name_(name), state_(ServiceState::CREATED),
              start_time_(std::chrono::steady_clock::now()) {}
        
        virtual ~BaseService() = default;
        
        virtual Status init() {
            state_ = ServiceState::INITIALIZED;
            return Status();
        }
        
        virtual Status start() {
            if (state_ != ServiceState::INITIALIZED) {
                return Status(false, "Service not initialized");
            }
            state_ = ServiceState::RUNNING;
            start_time_ = std::chrono::steady_clock::now();
            return Status();
        }
        
        virtual Status stop() {
            state_ = ServiceState::STOPPED;
            return Status();
        }
        
        virtual HealthStatus checkHealth() const {
            if (state_ == ServiceState::RUNNING) {
                return HealthStatus(true, "OK");
            }
            return HealthStatus(false, "Service Stopped");
        }
        
        virtual Status setConfig(const ServiceConfig& config) {
            config_ = config;
            return Status();
        }
        
        virtual ServiceConfig getConfig() const {
            return config_;
        }
        
        virtual ServiceMetrics getMetrics() const {
            auto now = std::chrono::steady_clock::now();
            auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
                now - start_time_).count();
            
            return {
                {"uptime_seconds", std::to_string(uptime)},
                {"state", std::to_string(static_cast<int>(state_))}
            };
        }
        
    protected:
        ServiceState state() const { return state_; }
        
    private:
        std::string name_;
        ServiceState state_;
        ServiceConfig config_;
        std::chrono::steady_clock::time_point start_time_;
    };
}
