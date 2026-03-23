#include "Window.hpp"

#include "UI/Canvas/Canvas.hpp"
#include "UI/GFX/Renderer/Renderer.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/IO/Input/Input.hpp"
#include "UI/Layout/LayoutHelpers.hpp"

#include <SDL3/SDL_gpu.h>

#include <stdexcept>

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/IO/Input/InputHelpers.hpp"

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

  InputHelpers::initSystemCursors();
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

  // Initialize input state with initial window size
  window->inputState.windowSize = {
    .x = static_cast<uint16_t>(width),
    .y = static_cast<uint16_t>(height)
  };

  // Create ui::Canvas
  const Rect bounds = getWindowBounds(window);
  window->canvas = createCanvas(&window->ecsRoot, bounds.x, bounds.y,
                                bounds.width, bounds.height, "Canvas");

  // Create ui::Renderer
  window->renderer = Renderer::createRenderer(window, &window->canvas);

  // Create app style
  window->appStyle = Style::getDefaultAppStyle();

  // Apply initial window layout
  relayout(window);
}

void ui::relayout(const Window *window, const uint16_t width, const uint16_t height)
{
  const auto &canvasRoot = window->canvas.entity;
  Vector2i windowSize = {width, height};

  if (width < 1 || height < 1) {
    const auto &inputState = window->inputState;
    windowSize = {
      .x = inputState.windowSize.x,
      .y = inputState.windowSize.y,
    };
  }

  auto baseComponent = canvasRoot.get_ref<ecs::BaseComponent>();
  baseComponent->rect = {
    .x = 0,
    .y = 0,
    .width = windowSize.x,
    .height = windowSize.y,
  };

  Layout::traverseAndApplyLayout(canvasRoot);
}

bool ui::updateWindow(Window *window)
{
  Input::pollEvents(&window->inputState, window);
  const auto& inputState = window->inputState;

  if (inputState.shouldQuit) {
    return false;
  }

  if (inputState.windowResized) {
    relayout(window);
  }

  InputHelpers::processEvents(window);
  Renderer::draw(window);

  return true;
}