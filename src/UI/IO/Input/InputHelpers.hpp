#pragma once

#include "Input.hpp"
#include "UI/Layout/LayoutTypes.hpp"

namespace ui {

class InputHelpers
{
public:
  static bool isMouseInRectComponent(const InputState &state, const Rect &rect)
  {
    const auto &[p_x, p_y] = state.mousePosition;

    return p_x >= rect.x &&
           p_x <= rect.x + rect.width &&
           p_y >= rect.y &&
           p_y <= rect.y + rect.height;
  }
};

} // namespace ui
