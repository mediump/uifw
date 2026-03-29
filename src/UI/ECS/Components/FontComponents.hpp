#pragma once

#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/RendererTypes.hpp"
#include "Utils.hpp"

#include <SDL3/SDL.h>

namespace ui {

UI_FORWARD_DECLARE_STRUCT(FontData);

enum TextHAlignment
{
  TextHAlignment_Left = 0,
  TextHAlignment_Center = 1,
  TextHAlignment_Right = 2,
};

enum TextVAlignment
{
  TextVAlignment_Top = 0,
  TextVAlignment_Middle = 1,
  TextVAlignment_Bottom = 2,
};

struct TextComponent
{
  const char *text = "TextComponent";
  FontData *font = nullptr;

  Color4f color = {1.0f, 1.0f, 1.0f, 1.0f};
  uint16_t pixelSize = 13;
  uint16_t padding = 0;

  bool isScrollable = false;
  ecs::Entity scrollbar = UI_NULL_ENTITY;
  float scrollPosition = 0.0f;

  TextHAlignment horizontalAlignment = TextHAlignment_Left;
  TextVAlignment verticalAlignment = TextVAlignment_Top;
  bool lineWrapping = true;

  bool needsUpdate = true;
};

} // namespace ui
