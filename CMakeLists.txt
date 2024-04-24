cmake_minimum_required(VERSION 3.27)

project(
    cl9
    VERSION 0.1
    LANGUAGES CXX
)

set(outputName teapot.out)
set(modulePath ${CMAKE_SOURCE_DIR}/modules)

add_executable(
    ${outputName}
    ${CMAKE_SOURCE_DIR}/backend/src/main.cpp
    ${modulePath}/cl9s/request_response.cpp
    ${modulePath}/cl9s/teapot_server.cpp
    ${modulePath}/cl9s/teapot_server_thread.cpp
    ${modulePath}/cl9s/teapot_server_static.cpp
)

target_include_directories(${outputName} PUBLIC ${CMAKE_SOURCE_DIR}/modules)
