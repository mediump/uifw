#pragma once

#include "UI/GFX/Renderer/RendererTypes.hpp"

#include <SDL3/SDL_keycode.h>
#include <cstdint>

namespace ui {

UI_FORWARD_DECLARE_STRUCT(ApplicationData);
UI_FORWARD_DECLARE_STRUCT(WindowData);

typedef SDL_Keycode KeyCode;

struct InputState
{
  bool shouldQuit = false;
  bool windowFocused = false;
  bool windowResized = false;
  bool windowMinimized = false;
  Vector2i windowSize = {0, 0};
  bool mouseMoved = false;
  Vector2i mousePosition = {0, 0};
  bool mouseDown = false;
  bool mouseUp = false;
  bool keyDown = false;
  bool modShift = false;
  bool modCtrl = false;
  KeyCode keyCode;
  Vector2f scrollDelta = {0.0f, 0.0f};
  uint64_t currentTime;
  std::string currentInputBuffer;
};

class Input
{
public:
  static void pollEvents(ApplicationData *app);

private:
  static void reset_input_state(WindowData *window);

  static Vector2i get_window_size(const WindowData *window);
  static Vector2i get_mouse_position(const WindowData *window);
};

} // namespace ui
