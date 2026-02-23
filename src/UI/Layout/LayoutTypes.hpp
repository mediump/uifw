#pragma once

namespace ui {

enum LayoutType
{
  LayoutType_None,
  LayoutType_Horizontal,
  LayoutType_Vertical
};

struct LayoutMargins
{
  uint16_t top;
  uint16_t right;
  uint16_t bottom;
  uint16_t left;
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
  LayoutType type;
  LayoutMargins margins;
  uint16_t spacing;
};

}