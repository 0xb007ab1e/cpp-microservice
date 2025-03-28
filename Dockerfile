# Build stage
FROM debian:bookworm-slim AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    protobuf-compiler \
    libprotobuf-dev \
    libgrpc-dev \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    nlohmann-json3-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
WORKDIR /src
COPY . .

# Build
RUN mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc) \
    && make install

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libprotobuf32 \
    libgrpc++1.51 \
    && rm -rf /var/lib/apt/lists/*

# Create service user
RUN useradd -r -s /bin/false service

# Create necessary directories
RUN mkdir -p /var/log/base-service /etc/base-service \
    && chown service:service /var/log/base-service

# Copy artifacts from builder
COPY --from=builder /usr/local/bin/base_service_exe /usr/local/bin/
COPY --from=builder /src/config/base_service.json /etc/base-service/

# Switch to service user
USER service

# Health check
HEALTHCHECK --interval=30s --timeout=30s --start-period=5s --retries=3 \
    CMD nc -z localhost 50051 || exit 1

# Set entrypoint
ENTRYPOINT ["/usr/local/bin/base_service_exe", "/etc/base-service/base_service.json"]
