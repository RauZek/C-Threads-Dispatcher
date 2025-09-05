cmake_minimum_required(VERSION 3.25)

project(assignment_c_threads LANGUAGES C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-Wall -Werror -Wextra -Wpedantic)

add_subdirectory(c_base_thread)

add_subdirectory(c_simple_process)

add_subdirectory(c_advanced_process)

add_subdirectory(dispatcher)

add_subdirectory(test_app)
