#CMAKE
cmake_minimum_required(VERSION 3.0)

project(rage_module VERSION 0.5 LANGUAGES C CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)

add_compile_options(-ftemplate-backtrace-limit=0)

set_property(TARGET rage  PROPERTY CXX_STANDARD 11)

add_executable(rage_test include/rage.cpp) 

#target_link_libraries(rage_test rage)
#add_test(Test_of_rage rage_test)
