#include "Window.h"

#include "uifw/Core/Utils/Log.h"
#include "uifw/Platform/Input.h"

#include <stdlib.h>

#include "uifw/Core/Utils/Time.h"
#include "uifw/GFX/Renderer/Renderer.h"
#include "uifw/UI/Canvas/Canvas.h"
#include "uifw/UI/Layout/Layout.h"

static void relayout_window_contents(const ui_Window *window)
{
  ui_ECS_resizeCanvasToWindowSize(window);
  ui_traverseAndApplyLayout(window->scene.world, &window->scene.root_canvas);
}

ui_Window *ui_createWindow(const ui_WindowParams params, ui_Application *app)
{
  // Re-allocate app window list array
  const size_t prevSize = app->numWindows;
  const size_t newSize = prevSize + 1;

  ui_Window *newArr = realloc(app->windowList, newSize * sizeof(ui_Window));

  if (newArr == nullptr) {
    free(newArr);
    ui_LogFatal("Unable to reallocate app window list.");
    return nullptr;
  }

  app->windowList = newArr;
  app->numWindows = newSize;

  // Create new window
  const size_t currentIndex = app->numWindows - 1;
  ui_Window *currentWindow = &app->windowList[currentIndex];

  ui_LogInfo("Creating window...");

  currentWindow->sdl_window =
    SDL_CreateWindow(params.title, params.width, params.height, SDL_WINDOW_RESIZABLE);

  if (!currentWindow->sdl_window) {
    ui_LogFatal("Failed to create window: %s", SDL_GetError());
    return nullptr;
  }

  SDL_ShowWindow(currentWindow->sdl_window);

  currentWindow->id = currentIndex;

  // Assert window created correctly
#if defined(UIFW_DEBUG)
  uint16_t width, height;
  ui_getWindowSize(currentWindow, &width, &height);

  ui_Assert(params.width == width, "Window width incorrect");
  ui_Assert(params.height == height, "Window height incorrect");
#endif

  // Zero initialize input state
  ui_resetWindowInputState(currentWindow, true);

  // Zero initialize time state
  ui_Time_init(&currentWindow->time_state);

  // Init ECS world
  currentWindow->scene.world = ecs_init();
  currentWindow->scene.root_canvas = UI_NULL_ENTITY;

  // Create canvas
  uint16_t canvasWidth, canvasHeight;
  ui_getWindowSize(currentWindow, &canvasWidth, &canvasHeight);

  const ui_LayoutPadding defaultPadding = {
    .left = 5,
    .right = 5,
    .top = 5,
    .bottom = 5,
  };
  constexpr uint16_t defaultSpacing = 5;

  currentWindow->scene.root_canvas = ui_ECS_createCanvas(
    LayoutType_Vertical, defaultPadding, defaultSpacing, currentWindow);

  // Initialize renderer
  ui_rendererCreate(currentWindow);

  return currentWindow;
}

void ui_updateWindow(ui_Window *window)
{
  const ui_InputState *inputState = &window->input_state;

  if (inputState->first_frame) {
    relayout_window_contents(window);
  }

  ui_Time_recordFrameStart(&window->time_state);

  // Poll input events
  ui_pollWindowEvents(window);

  if (inputState->mouse_down) {
    ui_LogInfo("Mouse down event");
  }

  if (inputState->window_resized) {
    ui_LogInfo("Window resized event");
    relayout_window_contents(window);
  }

  ui_rendererDraw(window);

  ui_Time_recordFrameEnd(&window->time_state);

  // Display FPS in titlebar in debug mode
#if defined(UIFW_DEBUG)
  char buffer[100];
  const uint16_t fps = window->time_state.fps;
  sprintf(buffer, "uifw [FPS: %i]", fps);

  SDL_SetWindowTitle(window->sdl_window, buffer);
#endif
}

void ui_destroyWindow(ui_Window *window)
{
  SDL_DestroyWindow(window->sdl_window);
  ui_LogInfo("Destroying window - ID: %i", window->id);
  window->id = 0;
}

void ui_getWindowPosition(const ui_Window *window, uint16_t *x, uint16_t *y)
{
  int outX, outY;
  SDL_GetWindowPosition(window->sdl_window, &outX, &outY);

  *x = (uint16_t)outX;
  *y = (uint16_t)outY;
}

void ui_getWindowSize(const ui_Window *window, uint16_t *width, uint16_t *height)
{
  int outWidth, outHeight;
  SDL_GetWindowSize(window->sdl_window, &outWidth, &outHeight);

  *width = (uint16_t)outWidth;
  *height = (uint16_t)outHeight;
}
