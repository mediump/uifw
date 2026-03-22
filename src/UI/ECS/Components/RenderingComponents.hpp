#pragma once
#include "UI/GFX/Renderer/RendererTypes.hpp"

namespace ui::ecs {

struct Color
{
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float a = 1.0f;

  bool operator==(const Color & color) const
  {
    return r == color.r && g == color.g && b == color.b && a == color.a;
  }
};

struct QuadRendererComponent
{
  Color color;
  Vector4i borderRadius = {0, 0, 0, 0};
  Color borderColor = {0.5f, 0.5f, 0.5f, 1.0f};
  Vector4f borderWidths = {0.0f, 0.0f, 0.0f, 0.0f};
  bool clipContents = true;
};

}