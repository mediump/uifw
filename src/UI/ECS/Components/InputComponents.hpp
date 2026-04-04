#pragma once

#include "UI/ECS/Entity/Entity.hpp"

#include <functional>

namespace ui {

enum CursorShape
{
  CursorShape_Default,
  CursorShape_Hide,
  CursorShape_Pointer,
  CursorShape_IBeam,
  CursorShape_Grab,
  CursorShape_Move,
  CursorShape_VerticalResize,
  CursorShape_HorizontalResize,
};

enum HoverState
{
  HoverState_Idle,
  HoverState_Hovered,
  HoverState_Clicked,
};

namespace ecs {

struct HoverHandlerComponent
{
  CursorShape cursorShape = CursorShape_Pointer;
  HoverState state = HoverState_Idle;
};

struct ButtonComponent
{
  std::function<void(const Entity &)> onClick = nullptr;
};

struct InputFieldComponent
{
  bool focused = false;
  Entity text = UI_NULL_ENTITY;
  Entity caret = UI_NULL_ENTITY;
  size_t cursorPos = 0;
  uint64_t lastInputTime = 0;
};

} // namespace ecs

} // namespace ui
