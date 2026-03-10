cmake_minimum_required(VERSION 4.0)
project(uifw)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(sample)