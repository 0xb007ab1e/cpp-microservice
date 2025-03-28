cmake_minimum_required(VERSION 3.10)
project(base_service VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add library
add_library(base_service
    src/base_service_server.cpp)
target_include_directories(base_service 
    PUBLIC include
    PRIVATE src)
target_link_libraries(base_service
    PUBLIC
    base_service_proto
    gRPC::grpc++
    gRPC::grpc++_reflection)

# Tests
enable_testing()
find_package(GTest REQUIRED)

# Unit tests
add_executable(base_service_test tests/base_service_test.cpp)
target_link_libraries(base_service_test
    PRIVATE
    base_service
    GTest::GTest
    GTest::Main)
add_test(NAME base_service_test COMMAND base_service_test)

# gRPC integration tests
add_executable(base_service_grpc_test tests/base_service_grpc_test.cpp)
target_link_libraries(base_service_grpc_test
    PRIVATE
    base_service
    GTest::GTest
    GTest::Main)
add_test(NAME base_service_grpc_test COMMAND base_service_grpc_test)
