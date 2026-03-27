#include "Input.hpp"

#include <SDL3/SDL.h>
#include <vector>

#include "UI/Window/Window.hpp"

using namespace ui;

// Helper to check if an event belongs to a specific window
static bool is_event_for_window(const SDL_Event &event, Uint32 windowID)
{
  switch (event.type) {
  case SDL_EVENT_WINDOW_RESIZED:
  case SDL_EVENT_WINDOW_MOVED:
  case SDL_EVENT_WINDOW_MINIMIZED:
  case SDL_EVENT_WINDOW_MAXIMIZED:
  case SDL_EVENT_WINDOW_FOCUS_GAINED:
  case SDL_EVENT_WINDOW_FOCUS_LOST:
  case SDL_EVENT_WINDOW_SHOWN:
  case SDL_EVENT_WINDOW_HIDDEN:
  case SDL_EVENT_WINDOW_EXPOSED:
  case SDL_EVENT_WINDOW_RESTORED:
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return event.window.windowID == windowID;
  case SDL_EVENT_MOUSE_MOTION:
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  case SDL_EVENT_MOUSE_BUTTON_UP:
  case SDL_EVENT_MOUSE_WHEEL:
    return event.motion.windowID == windowID;
  case SDL_EVENT_KEY_DOWN:
  case SDL_EVENT_KEY_UP:
    return event.key.windowID == windowID;
  case SDL_EVENT_QUIT:
    // Quit is global, applies to all windows
    return true;
  default:
    return true;
  }
}

static void process_event(InputState *inputState, const SDL_Event &event)
{
  switch (event.type) {
  case SDL_EVENT_QUIT:
    inputState->shouldQuit = true;
    break;
  case SDL_EVENT_WINDOW_MOUSE_ENTER:
    inputState->windowFocused = true;
    break;
  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    inputState->windowFocused = false;
    break;
  case SDL_EVENT_WINDOW_RESIZED:
    inputState->windowResized = true;
    inputState->windowSize = {.x = static_cast<uint16_t>(event.window.data1),
                              .y = static_cast<uint16_t>(event.window.data2)};
    break;
  case SDL_EVENT_MOUSE_MOTION:
    inputState->mouseMoved = true;
    inputState->mousePosition = {.x = static_cast<uint16_t>(event.motion.x),
                                 .y = static_cast<uint16_t>(event.motion.y)};
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

void Input::pollEvents(InputState *inputState, const WindowData *window)
{
  reset_input_state(inputState);

  const Uint32 windowID = SDL_GetWindowID(window->sdlWindow);

  // Collect all events and distribute them to appropriate windows
  // We use a static buffer to avoid allocation on every call
  static std::vector<SDL_Event> event_buffer;
  static std::vector<SDL_Event> other_window_events;

  event_buffer.clear();
  other_window_events.clear();

  // First, drain all events from the queue
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    event_buffer.push_back(event);
  }

  // Process events for this window, save others for re-queue
  for (const auto &e : event_buffer) {
    if (is_event_for_window(e, windowID)) {
      process_event(inputState, e);
    }
    else {
      other_window_events.push_back(e);
    }
  }

  // Re-queue events for other windows
  for (auto &e : other_window_events) {
    SDL_PushEvent(&e);
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
