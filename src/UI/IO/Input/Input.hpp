#pragma once

#include "UI/GFX/Renderer/RendererTypes.hpp"

namespace ui {

UI_FORWARD_DECLARE_STRUCT(Window);

struct InputState
{
  bool shouldQuit = false;
  bool windowResized = false;
  Vector2i windowSize;
  bool mouseMoved = false;
  Vector2i mousePosition;
  bool mouseDown = false;
  bool mouseUp = false;
};

class Input
{
public:
  static void pollEvents(InputState *inputState, const Window *window);

private:
  static void reset_input_state(InputState *inputState);

  static Vector2i get_window_size(const Window *window);

  static Vector2i get_mouse_position(const Window *window);
};

} // namespace ui
