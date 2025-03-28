#pragma once
#include "base_service.hpp"
#include "base_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>

namespace project {
    class BaseServiceServer final : public project::base::BaseService::Service {
    public:
        explicit BaseServiceServer(std::shared_ptr<BaseService> service)
            : service_(std::move(service)) {}

        grpc::Status Initialize(grpc::ServerContext*,
                              const project::base::Empty*,
                              project::base::Status* response) override;

        grpc::Status Start(grpc::ServerContext*,
                         const project::base::Empty*,
                         project::base::Status* response) override;

        grpc::Status Stop(grpc::ServerContext*,
                        const project::base::Empty*,
                        project::base::Status* response) override;

        grpc::Status CheckHealth(grpc::ServerContext*,
                               const project::base::Empty*,
                               project::base::HealthStatus* response) override;

        grpc::Status SetConfig(grpc::ServerContext*,
                             const project::base::ServiceConfig* request,
                             project::base::Status* response) override;

        grpc::Status GetConfig(grpc::ServerContext*,
                             const project::base::Empty*,
                             project::base::ServiceConfig* response) override;

        grpc::Status GetMetrics(grpc::ServerContext*,
                              const project::base::Empty*,
                              project::base::MetricsData* response) override;

    private:
        std::shared_ptr<BaseService> service_;
    };
}
