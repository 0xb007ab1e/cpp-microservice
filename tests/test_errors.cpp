#include <gtest/gtest.h>
#include "project/error_handling.hpp"
#include <regex>
#include <fstream>

using namespace project;

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::instance().set_log_file("test.log");
    }

    void TearDown() override {
        std::remove("test.log");
    }
};

TEST_F(ErrorHandlingTest, ValidationError) {
    auto error = make_validation_error(
        "Invalid input",
        {{"field", "username"}, {"constraint", "length"}}
    );
    EXPECT_EQ(error.type(), ErrorType::VALIDATION_ERROR);
    EXPECT_EQ(error.code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(error.severity(), ErrorSeverity::WARNING);
    EXPECT_STREQ(error.what(), "Invalid input");
    
    const auto& details = error.details();
    EXPECT_EQ(details.at("field"), "username");
    EXPECT_EQ(details.at("constraint"), "length");
}

TEST_F(ErrorHandlingTest, RuntimeError) {
    auto error = make_runtime_error(
        "Operation failed",
        {{"operation", "save"}, {"reason", "disk full"}}
    );
    EXPECT_EQ(error.type(), ErrorType::RUNTIME_ERROR);
    EXPECT_EQ(error.code(), ErrorCode::RUNTIME_FAILED);
    EXPECT_EQ(error.severity(), ErrorSeverity::ERROR);
    
    const auto& details = error.details();
    EXPECT_EQ(details.at("operation"), "save");
    EXPECT_EQ(details.at("reason"), "disk full");
}

TEST_F(ErrorHandlingTest, CriticalError) {
    auto error = make_critical_error(
        "Database connection lost",
        ErrorType::DATABASE_ERROR,
        {{"host", "primary"}, {"port", "5432"}}
    );
    EXPECT_EQ(error.type(), ErrorType::DATABASE_ERROR);
    EXPECT_EQ(error.code(), ErrorCode::DATABASE_FAILED);
    EXPECT_EQ(error.severity(), ErrorSeverity::CRITICAL);
    
    const auto& details = error.details();
    EXPECT_EQ(details.at("host"), "primary");
    EXPECT_EQ(details.at("port"), "5432");
}

TEST_F(ErrorHandlingTest, StackTrace) {
    auto error = make_runtime_error("Operation failed");
    error.add_stack_frame("main():123");
    error.add_stack_frame("process():456");
    
    const auto& trace = error.stack_trace();
    EXPECT_EQ(trace.size(), 2);
    EXPECT_EQ(trace[0], "main():123");
    EXPECT_EQ(trace[1], "process():456");
    
    std::string full_msg = error.full_message();
    EXPECT_TRUE(full_msg.find("Stack trace:") != std::string::npos);
    EXPECT_TRUE(full_msg.find("main():123") != std::string::npos);
}

TEST_F(ErrorHandlingTest, ErrorLogging) {
    make_critical_error("Test error", ErrorType::SYSTEM_ERROR);
    
    std::ifstream log_file("test.log");
    std::string log_content((std::istreambuf_iterator<char>(log_file)),
                          std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(log_content.find("[CRITICAL]") != std::string::npos);
    EXPECT_TRUE(log_content.find("Test error") != std::string::npos);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
