cmake_minimum_required(VERSION 3.27)


project(
    cl9
    VERSION 0.1
    LANGUAGES CXX
)


set(outputName teapot.out)
set(modulePath ${CMAKE_SOURCE_DIR}/modules)

add_compile_definitions(CONSOLE_LOG)
add_compile_definitions(PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
add_compile_definitions(PUBLICS="${CMAKE_SOURCE_DIR}/public")

add_executable(
    ${outputName}
    ${CMAKE_SOURCE_DIR}/backend/src/main.cpp
    ${modulePath}/cl9s/teapot.cpp
    ${modulePath}/cl9s/teapot_server.cpp
    ${modulePath}/cl9s/teapot_client_loop.cpp
    ${modulePath}/cl9s/request/request.cpp
    ${modulePath}/cl9s/response/response.cpp
    ${modulePath}/cl9s/content/content_types.cpp
    ${modulePath}/file/filereader.cpp
)

target_include_directories(${outputName} PUBLIC ${CMAKE_SOURCE_DIR}/modules)
