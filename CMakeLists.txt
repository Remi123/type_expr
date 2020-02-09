#CMAKE
cmake_minimum_required(VERSION 3.0)

message(STATUS "Type_Expr Library CMake Configuration")

project(type_expr VERSION 0.5)


add_library(type_expr INTERFACE)
target_include_directories(type_expr INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(type_expr INTERFACE
        cxx_variadic_templates
        cxx_constexpr
        cxx_alias_templates
        cxx_auto_type
        cxx_long_long_type
        cxx_right_angle_brackets
        cxx_static_assert
        cxx_uniform_initialization
        cxx_user_literals
        ${CPP_EXTRA_FEATURES})

# Testing type_expr
option( TYPE_EXPR_TEST 
        "Type_expr testing with CTest" 
        ON)
include(CTest)
add_subdirectory(test)




