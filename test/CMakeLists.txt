#CMAKE
cmake_minimum_required(VERSION 3.0) 
message(STATUS "Type_expr's test cmake configuration ")

option(TYPE_EXPR_TEST_MPL
	"Generate 'te' tests` "
	ON)
option(TYPE_EXPR_TEST_ARRAY
	"Generate 'te::array' tests` "
  ON)
option(TYPE_EXPR_TEST_TUP
	"Generate 'te::tup' tests` "
	ON)
option(TYPE_EXPR_TEST_VAR
	"Generate 'te::var' tests` "
	ON)

if (BUILD_TESTING AND TYPE_EXPR_TEST  )

	if(TYPE_EXPR_TEST_MPL)
        add_executable(type_expr_test expr_test.cpp)
        target_link_libraries(type_expr_test type_expr)
        add_test(Test_of_Type_Expr type_expr_test) 
    endif()
	if(TYPE_EXPR_TEST_ARRAY)
        add_executable(test_array array_test.cpp)
		set_target_properties(test_array
			PROPERTIES
			CXX_STANDARD 14)
        target_link_libraries(test_array type_expr)
        add_test(Test_of_MdArray test_array)
	endif()
	if(TYPE_EXPR_TEST_TUP)
        add_executable(test_tup tup_test.cpp)
		set_target_properties(test_tup
			PROPERTIES
			CXX_STANDARD 17)
        target_link_libraries(test_tup type_expr)
        add_test(Test_of_Tup test_tup)
	endif()
	if(TYPE_EXPR_TEST_VAR)
        add_executable(test_var var_test.cpp)
        target_link_libraries(test_var type_expr)
        add_test(Test_of_Var test_var)
    endif()

endif()
