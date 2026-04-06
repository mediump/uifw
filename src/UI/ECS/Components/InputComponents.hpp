#pragma once

#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/Text/TextTypes.hpp"

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

enum InputFieldState
{
  InputFieldState_Inactive,
  InputFieldState_Active,
};

struct InputFieldComponent
{
  bool focused = false;
  InputFieldState state = InputFieldState_Inactive;
  Entity text = UI_NULL_ENTITY;
  Entity caret = UI_NULL_ENTITY;
  Entity selection = UI_NULL_ENTITY;
  size_t cursorPos = 0;
  size_t selectionStart = 0;
  size_t selectionEnd = 0;
  uint64_t lastInputTime = 0;
  FontData *font = nullptr;
};

} // namespace ecs

} // namespace ui
