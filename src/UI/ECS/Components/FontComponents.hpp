#pragma once

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
  const char* text = "TextComponent";
  FontData* font = nullptr;

  Color4f color = {1.0f, 1.0f, 1.0f, 1.0f};
  uint16_t pixelSize = 13;

  TextHAlignment horizontalAlignment = TextHAlignment_Left;
  TextVAlignment verticalAlignment = TextVAlignment_Top;
  bool lineWrapping = true;

  bool needsUpdate = true;
};

}