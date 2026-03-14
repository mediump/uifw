#pragma once
#include <stdint.h>

/**
 * Rect type containing x position, y position, width, and height
 */
typedef struct
{
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} ui_Rect;

typedef struct
{
  uint16_t x;
  uint16_t y;
} ui_Vector2i;

typedef enum
{
  LayoutType_Vertical,
  LayoutType_Horizontal,
} ui_LayoutType;

typedef struct
{
  uint16_t top;
  uint16_t right;
  uint16_t bottom;
  uint16_t left;
} ui_LayoutPadding;