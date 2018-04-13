cmake_minimum_required(VERSION 3.5)
project(parser)

set(CMAKE_CXX_STANDARD 14)

# LLVM include
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(_LLVM_LIBS support demangle core)
message("[LLVM] include directories: ${LLVM_INCLUDE_DIRS}")
message("[LLVM] definitions: ${LLVM_DEFINITIONS}")
message("[LLVM] libs: ${_LLVM_LIBS}")
# end LLVM include

message("[CMAKE] cxx compiler: ${CMAKE_CXX_COMPILER}")
message("[CMAKE] cxx standard: ${CMAKE_CXX_STANDARD}")

set(MAIN_HEADERS
    src/lexer.hpp src/ast.hpp src/exceptions.hpp src/parser.hpp)
set(MAIN_SOURCES
    src/main.cpp src/lexer.cpp src/ast.cpp src/parser.cpp)

add_executable(simple ${MAIN_SOURCES} ${MAIN_HEADERS})

option(WITH_TEST "Build and run tests" OFF)
if (WITH_TEST)
    add_definitions(-DRUN_TEST)
    include_directories(/usr/include/cppunit)
    link_directories(/usr/lib/x86_64-linux-gnu)
    include_directories(tests/)
    add_subdirectory(tests/test_parser)
    target_link_libraries(simple parser_test)

    add_subdirectory(tests/test_ast)
    target_link_libraries(simple ast_test)

    target_link_libraries(simple cppunit)
endif()

# link LLVM libraries
target_link_libraries(simple ${_LLVM_LIBS})
