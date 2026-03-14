#include "Input.h"
#include "uifw/Platform/Window.h"

#include <SDL3/SDL.h>

void ui_resetWindowInputState(ui_Window *window)
{
  ui_InputState *inputState = &window->inputState;

  inputState->shouldQuit = false;
  inputState->windowResized = false;
  inputState->mouseMoved = false;
  inputState->mouseDown = false;
}

void ui_pollWindowEvents(ui_Window *window)
{
  ui_resetWindowInputState(window);

  ui_InputState *inputState = &window->inputState;
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      inputState->shouldQuit = true;
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      inputState->windowResized = true;
      inputState->windowSize.x = (uint16_t)event.window.data1;
      inputState->windowSize.y = (uint16_t)event.window.data2;
      break;
    case SDL_EVENT_MOUSE_MOTION:
      inputState->mouseMoved = true;
      inputState->mousePosition.x = (uint16_t)event.motion.x;
      inputState->mousePosition.y = (uint16_t)event.motion.y;
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      inputState->mouseDown = true;
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      inputState->mouseDown = false;
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
