cmake_minimum_required(VERSION 3.0.2)
project(tests)

add_executable(tests tests.cpp move_history_test.cpp)
target_link_libraries(tests chess)
target_include_directories(tests PRIVATE "${chess_SOURCE_DIR}/src/")
add_compile_options(tests)

