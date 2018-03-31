cmake_minimum_required(VERSION 3.5)
project(parser_test)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES test_parser.cpp)
set(HEADERS test_parser.h ../CppUnitCommon.hpp)

#link_directories(/usr/lib/x86_64-linux-gnu)

add_library(parser_test SHARED ${SOURCES} ${HEADERS})
