cmake_minimum_required(VERSION 3.5)
project(ast_test)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES test_ast.cpp)
set(HEADERS test_ast.hpp)

add_library(ast_test SHARED ${SOURCES} ${HEADERS})
