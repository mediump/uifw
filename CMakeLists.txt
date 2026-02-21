cmake_minimum_required(VERSION 3.5)
project(sdl3-ui-test)

add_subdirectory(extern)

set(CMAKE_CXX_STANDARD 20)

add_executable(sdl3-ui-test main.cpp
  src/UI/Window/Window.cpp
  src/UI/Window/Window.hpp
  src/Utils.hpp
  src/UI/GFX/Shader.hpp
  src/UI/GFX/Shader.cpp
)

target_link_libraries(sdl3-ui-test PUBLIC
  SDL3::SDL3-static
)

target_include_directories(sdl3-ui-test PUBLIC
  src
)
