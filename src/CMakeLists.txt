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
  uifw/Platform/Input.h
  uifw/Platform/Input.c
  uifw/UI/Canvas/Canvas.h
  uifw/UI/Canvas/Canvas.c
  uifw/UI/Layout/Layout.c
  uifw/UI/Layout/Layout.h
  uifw/Core/Containers/Queue.h
  uifw/Core/Containers/Queue.c
  uifw/GFX/Renderer/RendererTypes.h
  uifw/GFX/Renderer/Renderer.h
  uifw/GFX/Renderer/Renderer.c
  uifw/GFX/Renderer/RendererHelpers.h
  uifw/GFX/Renderer/RendererHelpers.c
  uifw/Platform/IO/FileUtils.h
  uifw/Platform/IO/FileUtils.c
  uifw/GFX/Shader.h
  uifw/GFX/Shader.c
  uifw/Core/Utils/Time.h
)

target_link_libraries(uifw PUBLIC
  flecs
  SDL3::SDL3-static
  cglm
  stb
)

target_include_directories(uifw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
