cmake_minimum_required(VERSION 3.10)
project(cpp_microservices VERSION 1.0)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Find required packages
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# Find gRPC plugin
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
message(STATUS "Using gRPC plugin: ${GRPC_CPP_PLUGIN}")

# Generate protobuf and gRPC files
get_filename_component(base_service_proto "proto/base_service/base_service.proto" ABSOLUTE)
get_filename_component(base_service_proto_path "${base_service_proto}" PATH)

# Generate protobuf files
set(base_service_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/base_service.pb.cc")
set(base_service_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/base_service.pb.h")
set(base_service_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/base_service.grpc.pb.cc")
set(base_service_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/base_service.grpc.pb.h")

add_custom_command(
    OUTPUT "${base_service_proto_srcs}" "${base_service_proto_hdrs}"
           "${base_service_grpc_srcs}" "${base_service_grpc_hdrs}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${base_service_proto_path}"
         --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
         "${base_service_proto}"
    DEPENDS "${base_service_proto}")

# Include generated files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Create library for generated protobuf/gRPC code
add_library(base_service_proto
    ${base_service_proto_srcs}
    ${base_service_proto_hdrs}
    ${base_service_grpc_srcs}
    ${base_service_grpc_hdrs})

target_link_libraries(base_service_proto
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc++_reflection)

# Add subdirectories
add_subdirectory(common/base-service)
add_subdirectory(services/base-service)

# Enable testing
enable_testing()
