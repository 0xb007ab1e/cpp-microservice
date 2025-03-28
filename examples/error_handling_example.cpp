#include "project/error_handling.hpp"
#include <iostream>

using namespace project;

// Example function demonstrating validation error
void validate_user_input(const std::string& username, const std::string& email) {
    if (username.length() < 3) {
        throw make_validation_error(
            "Username too short",
            {{"field", "username"}, 
             {"min_length", "3"},
             {"actual_length", std::to_string(username.length())}}
        );
    }

    if (email.find('@') == std::string::npos) {
        throw make_validation_error(
            "Invalid email format",
            {{"field", "email"}, 
             {"value", email}}
        );
    }
}

// Example function demonstrating runtime error
void process_data(const std::string& data) {
    try {
        // Simulate some processing
        if (data.empty()) {
            throw make_runtime_error(
                "Empty data provided",
                {{"operation", "process_data"}}
            );
        }
    } catch (const ProjectError& e) {
        // Add stack trace information
        auto& error = const_cast<ProjectError&>(e);
        error.add_stack_frame("process_data():42");
        throw;
    }
}

// Example function demonstrating critical error
void connect_database(const std::string& host, int port) {
    // Simulate database connection failure
    throw make_critical_error(
        "Failed to connect to database",
        ErrorType::DATABASE_ERROR,
        {{"host", host},
         {"port", std::to_string(port)},
         {"timeout", "30s"}}
    );
}

int main() {
    // Set up logging
    Logger::instance().set_log_file("example.log");

    try {
        // Example 1: Validation error
        log_info("Validating user input...");
        validate_user_input("ab", "invalid-email");
    } catch (const ProjectError& e) {
        std::cerr << "Example 1 - Validation Error:\n" << e.full_message() << "\n\n";
    }

    try {
        // Example 2: Runtime error with stack trace
        log_info("Processing data...");
        process_data("");
    } catch (const ProjectError& e) {
        std::cerr << "Example 2 - Runtime Error:\n" << e.full_message() << "\n\n";
    }

    try {
        // Example 3: Critical error
        log_info("Connecting to database...");
        connect_database("localhost", 5432);
    } catch (const ProjectError& e) {
        std::cerr << "Example 3 - Critical Error:\n" << e.full_message() << "\n\n";
    }

    return 0;
}
