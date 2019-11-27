#CMAKE
cmake_minimum_required(VERSION 3.0)

project(rage_module VERSION 0.5 LANGUAGES C CXX)

add_subdirectory(include)
add_subdirectory(test)

#set_property(TARGET rage  PROPERTY CXX_STANDARD 11)
add_executable(rage_test include/rage.cpp) 

target_link_libraries(rage_test rage)

