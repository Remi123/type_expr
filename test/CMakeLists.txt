#CMAKE
cmake_minimum_required(VERSION 3.0) 
message(STATUS "Type_expr's test cmake configuration ")

if (BUILD_TESTING AND TYPE_EXPR_TEST  )
        add_executable(type_expr_test type_expr.cpp)
        target_link_libraries(type_expr_test type_expr)
        add_test(Test_of_Type_Expr type_expr_test) 

        add_executable(test_tup test_type_tup.cpp)
        target_link_libraries(test_tup type_expr)
        add_test(Test_of_Tup test_tup)

        add_executable(test_var test_type_var.cpp)
        target_link_libraries(test_var type_expr)
        add_test(Test_of_Var test_var)

endif()
