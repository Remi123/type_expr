#CMAKE
cmake_minimum_required(VERSION 3.0)
message(STATUS "Hello Test")

if(BUILD_TESTING)
        add_executable(type_expr_test type_expr.cpp) 

        target_link_libraries(type_expr_test type_expr)
        add_test(Test_of_Type_Expr type_expr_test)       
        endif()


