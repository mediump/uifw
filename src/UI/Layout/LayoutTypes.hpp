#pragma once

#include <cstdint>

namespace ui {

enum LayoutType
{
  LayoutType_None,
  LayoutType_Horizontal,
  LayoutType_Vertical
};

struct LayoutMargins
{
  uint16_t top = 0;
  uint16_t right = 0;
  uint16_t bottom = 0;
  uint16_t left = 0;
};

struct Rect
{
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
};

struct LayoutComponent
{
  LayoutType type = LayoutType_None;
  LayoutMargins margins = {0, 0, 0, 0};
  uint16_t spacing = 0;
};

}