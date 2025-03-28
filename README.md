# cpp-microservice

High-performance C++ microservice for efficient data processing and real-time analytics

## Features

- Modern C++17 implementation
- gRPC service infrastructure
- Built-in metrics and health checks
- Docker containerization
- Comprehensive test suite
- CI/CD pipeline with GitHub Actions

## Prerequisites

- C++17 compatible compiler
- CMake 3.10+
- Docker
- gRPC and Protobuf
- nlohmann-json

## Quick Start

1. Clone the repository:
```bash
git clone https://github.com/0xb007ab1e/cpp-microservice.git
```

2. Build the project:
```bash
mkdir build && cd build
cmake ..
make
```

3. Run tests:
```bash
./common/base-service/base_service_test
./common/base-service/base_service_grpc_test
```

4. Run with Docker:
```bash
docker-compose up -d
```

## Project Structure

```
.
├── common/
│   └── base-service/       # Core service implementation
├── config/                 # Configuration files
├── proto/                  # Protocol buffer definitions
├── services/
│   └── base-service/       # Service executable
├── tests/                  # Test suite
└── docker/                 # Docker configuration
```

## Configuration

Service configuration is managed through JSON files in the `config/` directory.
Environment-specific configurations can be provided through environment variables.

## Development

1. Copy `.env.template` to `.env` and configure environment variables
2. Use `scripts/setup-dev.sh` to configure development environment
3. Follow coding standards in `CONTRIBUTING.md`

## Testing

The project includes unit tests and integration tests:
- Unit tests for core functionality
- Integration tests for gRPC endpoints
- Performance tests
- Docker container tests

## Security

- Secrets management using GitHub Secrets
- Environment variable based configuration
- Regular security updates
- Dependency scanning

## Contributing

Please read `CONTRIBUTING.md` for details on our code of conduct and development process.

## License

This project is licensed under the MIT License - see the `LICENSE` file for details.
