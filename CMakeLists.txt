cmake_minimum_required(VERSION 3.0.2)
project(chess)

macro(add_compile_options target)
        target_compile_options(${target} PRIVATE "-Wall")
        target_compile_options(${target} PRIVATE "-Wextra")
        target_compile_options(${target} PRIVATE "-std=c++17")
        target_compile_options(${target} PRIVATE "-O0")
endmacro()

add_library(chess src/chess.cpp src/sdl++.cpp)
add_compile_options(chess)
add_executable(chess-exe src/main.cpp)
add_compile_options(chess-exe)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${chess_SOURCE_DIR}/cmake")

find_package(SDL2)
find_package(SDL2_image)

if (NOT SDL2_FOUND)
        message(FATAL_ERROR "SDL2 not found.")
endif()

if (NOT SDL2_IMAGE_FOUND)
        message(FATAL_ERROR "SDL2_image not found.")
endif()

target_include_directories(chess PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(chess PRIVATE ${SDL2_LIBRARY})
target_include_directories(chess PRIVATE ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(chess PRIVATE ${SDL2_IMAGE_LIBRARY})
target_include_directories(chess PRIVATE "${chess_SOURCE_DIR}/src")
target_include_directories(chess-exe PRIVATE "${chess_SOURCE_DIR}/src")
target_link_libraries(chess-exe chess)

add_subdirectory(tests)

