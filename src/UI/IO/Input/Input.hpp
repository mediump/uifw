#pragma once

#include "UI/GFX/Renderer/RendererTypes.hpp"

namespace ui {

UI_FORWARD_DECLARE_STRUCT(WindowData);

struct InputState
{
  bool shouldQuit = false;
  bool windowFocused = false;
  bool windowResized = false;
  Vector2i windowSize = {0, 0};
  bool mouseMoved = false;
  Vector2i mousePosition = {0, 0};
  bool mouseDown = false;
  bool mouseUp = false;
};

class Input
{
public:
  static void pollEvents(InputState *inputState, const WindowData *window);

private:
  static void reset_input_state(InputState *inputState);

  static Vector2i get_window_size(const WindowData *window);

  static Vector2i get_mouse_position(const WindowData *window);
};

} // namespace ui
