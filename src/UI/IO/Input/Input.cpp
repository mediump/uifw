#include "Input.hpp"

#include <SDL3/SDL.h>
#include <vector>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "UI/Core/Application.hpp"
#include "UI/Window/Window.hpp"

using namespace ui;

static void process_event(ApplicationData *app, const SDL_Event &event)
{
  auto it = app->windows.find(event.window.windowID);
  if (it == app->windows.end()) {
    return;
  }

  InputState &inputState = it->second->inputState;

  switch (event.type) {
  case SDL_EVENT_QUIT:
    app->shouldQuit = true;
    break;
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    inputState.shouldQuit = true;
    break;
  case SDL_EVENT_WINDOW_MOUSE_ENTER:
    inputState.windowFocused = true;
    break;
  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    inputState.windowFocused = false;
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    inputState.windowResized = true;
    inputState.windowSize = {.x = static_cast<uint16_t>(event.window.data1),
                             .y = static_cast<uint16_t>(event.window.data2)};
    break;
  case SDL_EVENT_MOUSE_MOTION:
    inputState.mouseMoved = true;
    inputState.mousePosition = {.x = static_cast<uint16_t>(event.motion.x),
                                .y = static_cast<uint16_t>(event.motion.y)};
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    inputState.mouseDown = true;
    inputState.mouseButton = event.button.button;
    break;
  case SDL_EVENT_MOUSE_BUTTON_UP:
    inputState.mouseUp = true;
    inputState.mouseButton = event.button.button;
    break;
  case SDL_EVENT_MOUSE_WHEEL:
    inputState.scrollDelta = {static_cast<float>(event.wheel.integer_x),
                              static_cast<float>(event.wheel.integer_y)};
    break;
  case SDL_EVENT_KEY_DOWN:
    inputState.keyDown = true;
    inputState.keyCode = event.key.key;

    // FIXME: Handle case where one modifier key is up and the other
    // is still down
    switch (event.key.key) {
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
        inputState.modShift = true;
        break;
      case SDLK_LCTRL:
      case SDLK_RCTRL:
        inputState.modCtrl = true;
        break;
    }
    break;
  case SDL_EVENT_TEXT_INPUT:
    inputState.currentInputBuffer += event.text.text;
    break;
  case SDL_EVENT_KEY_UP:
    switch (event.key.key) {
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
        inputState.modShift = false;
        break;
      case SDLK_LCTRL:
      case SDLK_RCTRL:
        inputState.modCtrl = false;
        break;
    }
    break;
  case SDL_EVENT_WINDOW_MINIMIZED:
    inputState.windowMinimized = true;
    break;
  case SDL_EVENT_WINDOW_RESTORED:
    inputState.windowMinimized = false;
    break;
  case SDL_EVENT_WINDOW_MAXIMIZED:
    break;
  case SDL_EVENT_WINDOW_FOCUS_GAINED:
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    break;
  default:
    break;
  }
}

void Input::pollEvents(ApplicationData *app)
{
  // Reset all window input states
  for (auto &[id, window] : app->windows) {
    reset_input_state(window);
  }

  // Poll all events, send events to each window
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    process_event(app, event);
  }
}

void Input::reset_input_state(WindowData *window)
{
  // Reset per-frame parameters to their defaults
  auto &inputState = window->inputState;

  inputState.shouldQuit = false;
  inputState.windowResized = false;
  inputState.mouseMoved = false;
  inputState.mouseDown = false;
  inputState.mouseUp = false;
  inputState.mouseButton = 0;
  inputState.keyDown = false;
  inputState.scrollDelta = {0.0, 0.0};
  inputState.currentInputBuffer.clear();

  // Get current time
  inputState.currentTime = SDL_GetTicks();
}

Vector2i Input::get_window_size(const WindowData *window)
{
  const auto windowBounds = Window::getWindowBounds(window);

  return {
    .x = static_cast<uint16_t>(windowBounds.width),
    .y = static_cast<uint16_t>(windowBounds.height),
  };
}

Vector2i Input::get_mouse_position(const WindowData *window)
{
  float xPos, yPos;
  SDL_GetMouseState(&xPos, &yPos);

  return {
    .x = static_cast<uint16_t>(xPos),
    .y = static_cast<uint16_t>(yPos),
  };
}
