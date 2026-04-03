cmake_minimum_required(VERSION 3.5)
project(sdl3-ui-test)

add_subdirectory(extern)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_library(uifw STATIC
  src/UI/Window/Window.cpp
  src/UI/Window/Window.hpp
  src/Utils.hpp
  src/UI/GFX/Shader.hpp
  src/UI/GFX/Shader.cpp
  src/UI/Canvas/Canvas.cpp
  src/UI/Canvas/Canvas.hpp
  src/UI/ECS/ECSRoot/ECSRoot.cpp
  src/UI/ECS/ECSRoot/ECSRoot.hpp
  src/UI/ECS/Entity/Entity.cpp
  src/UI/ECS/Entity/Entity.hpp
  src/UI/ECS/Components/BaseComponent.hpp
  src/UI/Layout/LayoutHelpers.cpp
  src/UI/Layout/LayoutHelpers.hpp
  src/UI/Layout/LayoutTypes.hpp
  src/UI/ECS/Components/RenderingComponents.hpp
  src/UI/GFX/Renderer/Renderer.cpp
  src/UI/GFX/Renderer/Renderer.hpp
  src/UI/GFX/Renderer/RendererTypes.hpp
  src/UI/ECS/Components/FontComponents.hpp
  src/UI/GFX/Renderer/Text/TextTypes.hpp
  src/UI/IO/Text/FontLoader.cpp
  src/UI/IO/Text/FontLoader.hpp
  src/UI/GFX/Renderer/Text/TextHelpers.cpp
  src/UI/GFX/Renderer/Text/TextHelpers.hpp
  src/UI/IO/Image/Image.cpp
  src/UI/IO/Image/Image.hpp
  src/UI/Utils/FileUtils.hpp
  src/UI/Utils/StringUtils.hpp
  src/UI/GFX/Renderer/Text/TextRendererHelpers.cpp
  src/UI/GFX/Renderer/Text/TextRendererHelpers.hpp
  src/UI/Core/Application.cpp
  src/UI/Core/Application.hpp
  src/UI/Widgets/Text/ScrollArea.cpp
  src/UI/Widgets/Text/ScrollArea.hpp
  src/UI/Utils/MathUtils.hpp
  src/UI/Widgets/Text/InputField.cpp
  src/UI/Widgets/Text/InputField.hpp
)

target_link_libraries(uifw PUBLIC
  flecs
  SDL3::SDL3-static
  glm::glm
  nlohmann_json::nlohmann_json
  stb
)

target_include_directories(uifw PUBLIC
  src
)

add_executable(sdl3-ui-test
  main.cpp
  src/UI/IO/Input/Input.cpp
  src/UI/IO/Input/Input.hpp
  src/UI/ECS/Components/InputComponents.hpp
  src/UI/IO/Input/InputHelpers.hpp
  src/UI/ECS/Components/StyleComponents.hpp
  src/UI/IO/Input/InputHelpers.cpp
)

target_link_libraries(sdl3-ui-test PRIVATE
  uifw
)

add_subdirectory(tests)

# Compile shaders using SDL_shadercross (user install required)
add_custom_command(TARGET sdl3-ui-test
  TARGET sdl3-ui-test
  POST_BUILD
  COMMAND bash ${CMAKE_SOURCE_DIR}/res/shaders/compile_shaders.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/res/shaders
  COMMENT "Compiling Shaders..."
)

# Generate fonts with bundled msdf-atlas-gen
add_custom_command(TARGET sdl3-ui-test
  TARGET sdl3-ui-test
  POST_BUILD
  COMMAND bash ${CMAKE_SOURCE_DIR}/res/fonts/generate_msdf_font_atlases.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/res/fonts
  COMMENT "Generating MSDF font atlases..."
)

# Copy res/ directory
add_custom_command(
  TARGET sdl3-ui-test
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:sdl3-ui-test>/res"
  COMMENT "Copying assets to build directory..."
  VERBATIM
)
