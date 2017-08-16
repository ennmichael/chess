cmake_minimum_required(VERSION 3.0.2)
project(chess)

add_executable(chess src/main.cpp src/visual.cpp src/game.cpp)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses) # Change to REQUIRED
if (NOT ${CURSES_FOUND}) # Remove this
  message(FATAL_ERROR "Curses required")
endif()
target_link_libraries(chess ${CURSES_LIBRARIES})
target_include_directories(chess PRIVATE ${CURSES_INCLUDE_DIR})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
target_link_libraries(chess ${Boost_LIBRARIES})
target_include_directories(chess PRIVATE ${Boost_INCLUDE_DIRS})

target_include_directories(chess PRIVATE "./include")

if (CMAKE_COMPILER_IS_GNUCC)
  target_compile_options(chess PRIVATE "-Wall")
  target_compile_options(chess PRIVATE "-Wextra")
  target_compile_options(chess PRIVATE "-std=c++17")
endif()

