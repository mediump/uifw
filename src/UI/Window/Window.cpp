#include "Window.hpp"

#include "UI/Canvas/Canvas.hpp"
#include "UI/GFX/Renderer/Renderer.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/Layout/LayoutHelpers.hpp"

#include <SDL3/SDL_gpu.h>

#include <stdexcept>

#include "UI/ECS/Components/BaseComponent.hpp"

void ui::initPlatform()
{
  const int version = SDL_GetVersion();
  const char *sdlRevision = SDL_GetRevision();

  SDL_Log("Initializing platform...\n");
  SDL_Log(" > SDL Version: %i.%i %s\n", SDL_VERSIONNUM_MAJOR(version),
          SDL_VERSIONNUM_MINOR(version), sdlRevision);

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    throw std::runtime_error("Unable to initialize SDL.");
  }
}

ui::Rect ui::getWindowBounds(const Window *window)
{
  int x, y, width, height;

  SDL_GetWindowPosition(window->ptr, &x, &y);
  SDL_GetWindowSize(window->ptr, &width, &height);

  return Rect {
    .x = static_cast<uint16_t>(x),
    .y = static_cast<uint16_t>(y),
    .width = static_cast<uint16_t>(width),
    .height = static_cast<uint16_t>(height)
  };
}

void ui::initializeWindow(const char *title,
                          const int width,
                          const int height,
                          Window *window)
{
  // Create window
  SDL_Log("Initializing window...\n");
  window->ptr = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);

  if (window->ptr == nullptr) {
    throw std::runtime_error("Unable to create a window.");
  }

  SDL_Log(" > Window size: [%i, %i]\n", width, height);

  // Create ui::Canvas
  const Rect bounds = getWindowBounds(window);
  window->canvas = createCanvas(&window->ecsRoot, bounds.x, bounds.y,
                                bounds.width, bounds.height, "Canvas");

  // Create ui::Renderer
  window->renderer = Renderer::createRenderer(window, &window->canvas);
}

void ui::relayout(const Window *window)
{
  const auto windowBounds = ui::getWindowBounds(window);
  const auto canvasRoot = window->canvas.entity;

  auto baseComponent = canvasRoot.get_ref<ui::ecs::BaseComponent>();
  baseComponent->rect = {
    .x = 0,
    .y = 0,
    .width = windowBounds.width,
    .height = windowBounds.height
  };

  Layout::traverseAndApplyLayout(canvasRoot);
}

bool ui::updateWindow(Window *window)
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_WINDOW_RESIZED:
        window->needsRelayout = true;
        break;
      case SDL_EVENT_QUIT:
        SDL_DestroyWindow(window->ptr);
        return false;
      default:
        break;
    }
  }

  if (window->needsRelayout) {
    relayout(window);
    window->needsRelayout = false;
  }

  Renderer::draw(window);

  return true;
}