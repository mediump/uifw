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