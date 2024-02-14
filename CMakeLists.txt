cmake_minimum_required(VERSION 3.27)

project(
    cl9
    VERSION 0.1
    LANGUAGES CXX
)

set(outputName ${PROJECT_NAME}.out)

add_executable(${outputName} ${CMAKE_SOURCE_DIR}/backend/src/main.cpp)
target_include_directories(${outputName} PUBLIC ${CMAKE_SOURCE_DIR}/modules)