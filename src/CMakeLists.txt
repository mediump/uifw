cmake_minimum_required(VERSION 4.0)

add_library(uifw STATIC
  uifw/Core/Application.c
  uifw/Core/Application.h
  uifw/GFX/GFX.h
  uifw/GFX/GFX.c
  uifw/Core/Utils/Log.h
  uifw/Core/Utils/Defs.h
  uifw/uifw.h
  uifw/Platform/Window.h
  uifw/Platform/Window.c
  uifw/Core/Types.h
  uifw/Core/Utils/Memory.h
  uifw/ECS/Entity.h
  uifw/ECS/World.h
  uifw/Core/Coordinates.h
  uifw/ECS/Entity.c
  uifw/ECS/Components.h
)

target_link_libraries(uifw PUBLIC
  flecs
  SDL3::SDL3-static
  glm::glm
  nlohmann_json::nlohmann_json
  stb
)

target_include_directories(uifw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
