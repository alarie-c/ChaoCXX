cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address")

project(CHAOCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

add_executable(chaocpp
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
    src/parser.cpp
    src/errors.cpp
    src/ast.cpp
    src/cbc.cpp
)

target_include_directories(chaocpp PRIVATE src)