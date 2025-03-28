#include "base_service_server.hpp"

namespace project {
    grpc::Status BaseServiceServer::Initialize(grpc::ServerContext*,
                                             const project::base::Empty*,
                                             project::base::Status* response) {
        auto status = service_->init();
        response->set_ok(status.isOk());
        response->set_message(status.message());
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::Start(grpc::ServerContext*,
                                        const project::base::Empty*,
                                        project::base::Status* response) {
        auto status = service_->start();
        response->set_ok(status.isOk());
        response->set_message(status.message());
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::Stop(grpc::ServerContext*,
                                       const project::base::Empty*,
                                       project::base::Status* response) {
        auto status = service_->stop();
        response->set_ok(status.isOk());
        response->set_message(status.message());
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::CheckHealth(grpc::ServerContext*,
                                              const project::base::Empty*,
                                              project::base::HealthStatus* response) {
        auto health = service_->checkHealth();
        response->set_healthy(health.isHealthy());
        response->set_status(health.getStatus());
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::SetConfig(grpc::ServerContext*,
                                            const project::base::ServiceConfig* request,
                                            project::base::Status* response) {
        ServiceConfig config;
        for (const auto& item : request->items()) {
            config[item.key()] = item.value();
        }
        
        auto status = service_->setConfig(config);
        response->set_ok(status.isOk());
        response->set_message(status.message());
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::GetConfig(grpc::ServerContext*,
                                            const project::base::Empty*,
                                            project::base::ServiceConfig* response) {
        auto config = service_->getConfig();
        for (const auto& [key, value] : config) {
            auto* item = response->add_items();
            item->set_key(key);
            item->set_value(value);
        }
        return grpc::Status::OK;
    }

    grpc::Status BaseServiceServer::GetMetrics(grpc::ServerContext*,
                                             const project::base::Empty*,
                                             project::base::MetricsData* response) {
        auto metrics = service_->getMetrics();
        for (const auto& [key, value] : metrics) {
            auto* item = response->add_metrics();
            item->set_key(key);
            item->set_value(value);
        }
        return grpc::Status::OK;
    }
}
