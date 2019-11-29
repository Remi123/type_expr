#CMAKE
cmake_minimum_required(VERSION 3.0)

add_executable(test_of_rage ../include/rage.cpp)


target_link_libraries(test_of_rage rage)
add_test(Test_of_rage test_of_rage)
