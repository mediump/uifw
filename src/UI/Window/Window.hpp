#pragma once

#include "UI/Canvas/Canvas.hpp"
#include "UI/GFX/Renderer/Renderer.hpp"
#include "UI/ECS/Components/StyleComponents.hpp"

#include <SDL3/SDL.h>

#include "UI/IO/Input/Input.hpp"
#include "UI/Layout/LayoutTypes.hpp"

namespace ui {

struct Window
{
  bool needsRelayout = true;
  SDL_Window *ptr = nullptr;
  RendererData renderer;
  Canvas canvas;
  ecs::ECSRoot ecsRoot;
  InputState inputState;
  AppStyle appStyle;
};

void initPlatform();

[[nodiscard]] Rect getWindowBounds(const Window *window);

[[nodiscard]] Canvas createCanvasForWindow(const Window &window,
                                           const ecs::ECSRoot *root);

void initializeWindow(const char *title, int width, int height, Window *window);

void relayout(const Window *window, uint16_t width = 0, uint16_t height = 0);

bool updateWindow(Window *window);

}  // namespace ui