#include "Window.hpp"

#include "UI/Canvas/Canvas.hpp"
#include "UI/Core/Application.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/GFX/Renderer/Renderer.hpp"
#include "UI/IO/Input/Input.hpp"
#include "UI/IO/Input/InputHelpers.hpp"
#include "UI/Layout/LayoutHelpers.hpp"

#include <stdexcept>

using namespace ui;

Rect Window::getWindowBounds(const WindowData *window)
{
  int x, y, width, height;

  SDL_GetWindowPosition(window->sdlWindow, &x, &y);
  SDL_GetWindowSize(window->sdlWindow, &width, &height);

  return Rect{.x = static_cast<uint16_t>(x),
              .y = static_cast<uint16_t>(y),
              .width = static_cast<uint16_t>(width),
              .height = static_cast<uint16_t>(height)};
}

WindowData *Window::initializeWindow(const char *title,
                                     const int width,
                                     const int height,
                                     ApplicationData *app)
{
  auto window = new WindowData();

  // Create window
  SDL_Log("Initializing window...\n");
  window->sdlWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);

  if (window->sdlWindow == nullptr) {
    throw std::runtime_error("Unable to create a window.");
  }

  SDL_Log(" > Window size: [%i, %i]\n", width, height);

  // Set window icon
  if (SDL_Surface *icon = SDL_LoadPNG("res/icons/app_icon.png")) {
    SDL_SetWindowIcon(window->sdlWindow, icon);
    SDL_DestroySurface(icon);
  }

  // Initialize input state with initial window size
  window->inputState.windowSize = {.x = static_cast<uint16_t>(width),
                                   .y = static_cast<uint16_t>(height)};

  // Create ECSWorld
  window->ecsRoot.world = new flecs::world();

  // Create ui::Canvas
  const Rect bounds = getWindowBounds(window);
  window->canvas = createCanvas(&window->ecsRoot, bounds.x, bounds.y, bounds.width,
                                bounds.height, "Canvas");

  // Create ui::Renderer
  window->renderer = Renderer::createRenderer(window, &window->canvas);

  // Apply initial window layout
  relayout(window);

  // Record window in app window list
  window->app = app;
  app->windows.emplace_back(window);

  return window;
}

void Window::relayout(const WindowData *window,
                      const uint16_t width,
                      const uint16_t height)
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

bool Window::updateWindow(WindowData *window)
{
  Input::pollEvents(&window->inputState, window);
  const auto &inputState = window->inputState;

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

void Window::destroy(const WindowData *window)
{
  SDL_DestroyWindow(window->sdlWindow);
}
