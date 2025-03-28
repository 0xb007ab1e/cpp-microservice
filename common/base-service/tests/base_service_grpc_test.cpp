#include <gtest/gtest.h>
#include "base_service_server.hpp"
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include <thread>
#include <memory>
#include <chrono>

namespace project {
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
}

class BaseServiceGrpcTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create and start gRPC server
        baseService = std::make_shared<project::TestService>();
        grpcServer = std::make_unique<project::BaseServiceServer>(baseService);
        
        grpc::ServerBuilder builder;
        // Let the system choose a free port
        std::string server_address("localhost:0");
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &port);
        builder.RegisterService(grpcServer.get());
        
        server = builder.BuildAndStart();
        ASSERT_NE(server, nullptr);
        
        // Create channel
        server_address = "localhost:" + std::to_string(port);
        channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
        ASSERT_NE(channel, nullptr);
        
        // Create stub
        stub = project::base::BaseService::NewStub(channel);
        ASSERT_NE(stub, nullptr);
        
        // Wait for server to start
        ASSERT_TRUE(channel->WaitForConnected(
            std::chrono::system_clock::now() + std::chrono::seconds(5)));
    }
    
    void TearDown() override {
        if (server) {
            server->Shutdown();
            server->Wait();
        }
        stub.reset();
        channel.reset();
        grpcServer.reset();
        baseService.reset();
    }
    
    std::shared_ptr<project::TestService> baseService;
    std::unique_ptr<project::BaseServiceServer> grpcServer;
    std::unique_ptr<grpc::Server> server;
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<project::base::BaseService::Stub> stub;
    int port;
};

TEST_F(BaseServiceGrpcTest, InitializeTest) {
    grpc::ClientContext context;
    project::base::Empty request;
    project::base::Status response;
    
    auto status = stub->Initialize(&context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(response.ok());
}

TEST_F(BaseServiceGrpcTest, StartTest) {
    {
        grpc::ClientContext init_context;
        project::base::Empty request;
        project::base::Status response;
        EXPECT_TRUE(stub->Initialize(&init_context, request, &response).ok());
    }
    
    grpc::ClientContext start_context;
    project::base::Empty request;
    project::base::Status response;
    
    auto status = stub->Start(&start_context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(response.ok());
}

TEST_F(BaseServiceGrpcTest, ConfigTest) {
    project::base::ServiceConfig config;
    auto* item = config.add_items();
    item->set_key("test_key");
    item->set_value("test_value");
    
    {
        grpc::ClientContext set_context;
        project::base::Status response;
        auto status = stub->SetConfig(&set_context, config, &response);
        EXPECT_TRUE(status.ok());
        EXPECT_TRUE(response.ok());
    }
    
    {
        grpc::ClientContext get_context;
        project::base::Empty request;
        project::base::ServiceConfig response;
        auto status = stub->GetConfig(&get_context, request, &response);
        EXPECT_TRUE(status.ok());
        EXPECT_EQ(response.items_size(), 1);
        EXPECT_EQ(response.items(0).key(), "test_key");
        EXPECT_EQ(response.items(0).value(), "test_value");
    }
}

TEST_F(BaseServiceGrpcTest, HealthCheckTest) {
    {
        grpc::ClientContext init_context;
        project::base::Empty request;
        project::base::Status response;
        EXPECT_TRUE(stub->Initialize(&init_context, request, &response).ok());
    }
    
    {
        grpc::ClientContext start_context;
        project::base::Empty request;
        project::base::Status response;
        EXPECT_TRUE(stub->Start(&start_context, request, &response).ok());
    }
    
    grpc::ClientContext health_context;
    project::base::Empty request;
    project::base::HealthStatus response;
    
    auto status = stub->CheckHealth(&health_context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(response.healthy());
    EXPECT_EQ(response.status(), "OK");
}

TEST_F(BaseServiceGrpcTest, MetricsTest) {
    {
        grpc::ClientContext init_context;
        project::base::Empty request;
        project::base::Status response;
        EXPECT_TRUE(stub->Initialize(&init_context, request, &response).ok());
    }
    
    {
        grpc::ClientContext start_context;
        project::base::Empty request;
        project::base::Status response;
        EXPECT_TRUE(stub->Start(&start_context, request, &response).ok());
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    grpc::ClientContext metrics_context;
    project::base::Empty request;
    project::base::MetricsData response;
    
    auto status = stub->GetMetrics(&metrics_context, request, &response);
    EXPECT_TRUE(status.ok());
    EXPECT_GT(response.metrics_size(), 0);
    
    bool found_uptime = false;
    bool found_state = false;
    for (const auto& metric : response.metrics()) {
        if (metric.key() == "uptime_seconds") {
            found_uptime = true;
            EXPECT_GT(std::stod(metric.value()), 0.0);
        }
        if (metric.key() == "state") {
            found_state = true;
        }
    }
    EXPECT_TRUE(found_uptime);
    EXPECT_TRUE(found_state);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
