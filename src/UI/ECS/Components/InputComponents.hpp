#pragma once

#include "UI/ECS/Entity/Entity.hpp"

#include <functional>

namespace ui {

enum CursorShape
{
  CursorShape_Default,
  CursorShape_Hide,
  CursorShape_Pointer,
  CursorShape_Grab,
  CursorShape_Move,
  CursorShape_VerticalResize,
  CursorShape_HorizontalResize,
};

namespace ecs {

struct HoverHandlerComponent
{
  CursorShape cursorShape = CursorShape_Pointer;
};

struct ButtonComponent
{
  std::function<void(const Entity &)> onClick = nullptr;
};

} // namespace ecs

} // namespace ui
