#include "Input.hpp"

#include <SDL3/SDL.h>

#include "UI/Window/Window.hpp"

using namespace ui;

void Input::pollEvents(InputState *inputState, const Window *window)
{
  reset_input_state(inputState);

  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      inputState->shouldQuit = true;
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      inputState->windowResized = true;
      inputState->windowSize = get_window_size(window);
      break;
    case SDL_EVENT_MOUSE_MOTION:
      inputState->mouseMoved = true;
      inputState->mousePosition = get_mouse_position(window);
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      inputState->mouseDown = true;
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      inputState->mouseUp = true;
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      break;
    case SDL_EVENT_KEY_DOWN:
      break;
    case SDL_EVENT_KEY_UP:
      break;
    case SDL_EVENT_WINDOW_MINIMIZED:
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
}

void Input::reset_input_state(InputState *inputState)
{
  inputState->shouldQuit = false;
  inputState->windowResized = false;
  inputState->mouseMoved = false;
  inputState->mouseDown = false;
  inputState->mouseUp = false;
}

Vector2i Input::get_window_size(const Window *window)
{
  const auto windowBounds = getWindowBounds(window);

  return {
    .x = static_cast<uint16_t>(windowBounds.width),
    .y = static_cast<uint16_t>(windowBounds.height),
  };
}

Vector2i Input::get_mouse_position(const Window *window)
{
  float xPos, yPos;
  SDL_GetMouseState(&xPos, &yPos);

  return {
    .x = static_cast<uint16_t>(xPos),
    .y = static_cast<uint16_t>(yPos),
  };
}
