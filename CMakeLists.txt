#CMAKE
cmake_minimum_required(VERSION 3.0)

project(rage_module VERSION 0.5 LANGUAGES C CXX)

add_subdirectory(include)

enable_testing()
add_subdirectory(test)



#set_property(TARGET rage  PROPERTY CXX_STANDARD 11)

