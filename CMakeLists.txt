#CMAKE
cmake_minimum_required(VERSION 3.0)

message(STATUS "Type_Expr Library CMake Configuration")

project(type_expr VERSION 0.5)


add_library(type_expr INTERFACE)
target_include_directories(type_expr INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Testing type_expr
option( TYPE_EXPR_TEST 
        "Type_expr testing with CTest" 
        OFF)
include(CTest)
add_subdirectory(test)

