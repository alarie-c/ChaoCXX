cmake_minimum_required(VERSION 3.10)

project(CHAOCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

add_executable(chaocpp
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
)

target_include_directories(chaocpp PRIVATE src)