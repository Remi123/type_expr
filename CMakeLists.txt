#CMAKE
cmake_minimum_required(VERSION 3.0)

project(Type_Expr_module VERSION 0.5 LANGUAGES C CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)

add_compile_options(-ftemplate-backtrace-limit=0)

set_property(TARGET Type_Expr  PROPERTY CXX_STANDARD 11)

add_executable(type_expr_test include/type_expr.cpp) 

target_link_libraries(type_expr_test Type_Expr)
add_test(Test_of_Type_Expr type_expr_test)
