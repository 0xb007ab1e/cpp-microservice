cmake_minimum_required(VERSION 3.10)
project(base_service_exe VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(nlohmann_json REQUIRED)

# Add executable
add_executable(base_service_exe
    src/main.cpp)

target_link_libraries(base_service_exe
    PRIVATE
    base_service
    nlohmann_json::nlohmann_json)

# Install
install(TARGETS base_service_exe
    RUNTIME DESTINATION bin)

install(FILES ${CMAKE_SOURCE_DIR}/config/base_service.json
    DESTINATION etc/base-service)
