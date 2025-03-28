#include <gtest/gtest.h>
#include "base_service.hpp"
#include <chrono>
#include <thread>

using namespace project;

class TestService : public BaseService {
public:
    TestService() : BaseService("test-service") {}
    
    Status init() override {
        return BaseService::init();
    }
    
    Status start() override {
        return BaseService::start();
    }
    
    Status stop() override {
        return BaseService::stop();
    }
    
    ServiceState getState() const {
        return state();
    }
};

class BaseServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<TestService>();
    }
    
    void TearDown() override {
        if (service && service->getState() != ServiceState::STOPPED) {
            service->stop();
        }
    }
    
    std::unique_ptr<TestService> service;
};

TEST_F(BaseServiceTest, InitializationTest) {
    EXPECT_EQ(service->getState(), ServiceState::CREATED);
    auto status = service->init();
    EXPECT_TRUE(status.isOk());
    EXPECT_EQ(service->getState(), ServiceState::INITIALIZED);
}

TEST_F(BaseServiceTest, StartTest) {
    EXPECT_TRUE(service->init().isOk());
    auto status = service->start();
    EXPECT_TRUE(status.isOk());
    EXPECT_EQ(service->getState(), ServiceState::RUNNING);
}

TEST_F(BaseServiceTest, StopTest) {
    EXPECT_TRUE(service->init().isOk());
    EXPECT_TRUE(service->start().isOk());
    auto status = service->stop();
    EXPECT_TRUE(status.isOk());
    EXPECT_EQ(service->getState(), ServiceState::STOPPED);
}

TEST_F(BaseServiceTest, HealthCheckTest) {
    EXPECT_TRUE(service->init().isOk());
    EXPECT_TRUE(service->start().isOk());
    
    auto health = service->checkHealth();
    EXPECT_TRUE(health.isHealthy());
    EXPECT_EQ(health.getStatus(), "OK");
    
    EXPECT_TRUE(service->stop().isOk());
    health = service->checkHealth();
    EXPECT_FALSE(health.isHealthy());
    EXPECT_EQ(health.getStatus(), "Service Stopped");
}

TEST_F(BaseServiceTest, ConfigurationTest) {
    ServiceConfig config{
        {"port", "8080"},
        {"host", "localhost"},
        {"max_connections", "100"}
    };
    
    EXPECT_TRUE(service->setConfig(config).isOk());
    auto retrievedConfig = service->getConfig();
    EXPECT_EQ(retrievedConfig["port"], "8080");
    EXPECT_EQ(retrievedConfig["host"], "localhost");
    EXPECT_EQ(retrievedConfig["max_connections"], "100");
}

TEST_F(BaseServiceTest, MetricsTest) {
    EXPECT_TRUE(service->init().isOk());
    EXPECT_TRUE(service->start().isOk());
    
    auto metrics = service->getMetrics();
    EXPECT_NE(metrics.find("uptime_seconds"), metrics.end());
    EXPECT_NE(metrics.find("state"), metrics.end());
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto newMetrics = service->getMetrics();
    EXPECT_GT(std::stod(newMetrics["uptime_seconds"]),
            std::stod(metrics["uptime_seconds"]));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
