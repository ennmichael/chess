cmake_minimum_required(VERSION 3.0.2)
project(tests)

add_executable(tests tests.cpp)
target_link_libraries(tests chess)
add_compile_options(tests)

