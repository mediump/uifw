#pragma once

#include "UI/Canvas/Canvas.hpp"
#include "UI/ECS/Components/StyleComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/Renderer.hpp"

#include <SDL3/SDL.h>

#include "UI/IO/Input/Input.hpp"
#include "UI/Layout/LayoutTypes.hpp"

namespace ui {

struct ApplicationData;

struct WindowData
{
  bool needsRelayout = true;
  SDL_Window *sdlWindow = nullptr;
  ApplicationData *app = nullptr;
  RendererData renderer;
  Canvas canvas;
  ecs::ECSRoot ecsRoot;
  InputState inputState;
  ecs::Entity contextMenu = UI_NULL_ENTITY;
};

class Window
{
public:
  [[nodiscard]] static Rect getWindowBounds(const WindowData *window);

  static WindowData *initializeWindow(const char *title,
                                      uint16_t width,
                                      uint16_t height,
                                      ApplicationData *app);

  static void relayout(WindowData *window, uint16_t width = 0, uint16_t height = 0);

  static bool updateWindow(WindowData *window);

  static void destroy(const WindowData *window);
};

} // namespace ui
