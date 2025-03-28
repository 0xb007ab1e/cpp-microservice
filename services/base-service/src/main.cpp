#include "base_service_server.hpp"
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <string>
#include <memory>

using json = nlohmann::json;

namespace {
    volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler(int signal) {
    gSignalStatus = signal;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    // Load configuration
    std::ifstream config_file(argv[1]);
    if (!config_file.is_open()) {
        std::cerr << "Failed to open config file: " << argv[1] << std::endl;
        return 1;
    }

    json config;
    try {
        config_file >> config;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse config file: " << e.what() << std::endl;
        return 1;
    }

    // Set up signal handling
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Create service instance
    auto service = std::make_shared<project::BaseService>(
        config["service"]["name"].get<std::string>());

    // Initialize logging
    service->setConfig({
        {"log_file", config["service"]["logging"]["file"].get<std::string>()},
        {"log_level", config["service"]["logging"]["level"].get<std::string>()}
    });

    // Create and start gRPC server
    std::string server_address = 
        config["service"]["host"].get<std::string>() + ":" +
        std::to_string(config["service"]["port"].get<int>());

    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    project::BaseServiceServer service_impl(service);
    builder.RegisterService(&service_impl);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Initialize and start the service
    auto status = service->init();
    if (!status.isOk()) {
        std::cerr << "Failed to initialize service: " << status.message() << std::endl;
        return 1;
    }

    status = service->start();
    if (!status.isOk()) {
        std::cerr << "Failed to start service: " << status.message() << std::endl;
        return 1;
    }

    // Wait for shutdown signal
    while (gSignalStatus == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Shutting down..." << std::endl;

    // Stop the service and server
    server->Shutdown();
    service->stop();

    return 0;
}
