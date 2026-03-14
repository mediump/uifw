#pragma once

#include "uifw/Core/Coordinates.h"
#include "uifw/ECS/Entity.h"

#include <stddef.h>

/**
 * Struct containing the transform relationship of an entity
 */
typedef struct
{
  size_t nChildren;
  ui_ECS_Entity first;
  ui_ECS_Entity prev;
  ui_ECS_Entity next;
  ui_ECS_Entity parent;
} ui_ECS_TransformRelComponent;

/**
 * Base component (assigned to each entity in the world)
 */
typedef struct
{
  ui_Rect coords;
  uint16_t minWidth;
  uint16_t minHeight;
  uint16_t maxWidth;
  uint16_t maxHeight;
  uint16_t zOrder;
  bool needsUpdate;
} ui_ECS_BaseComponent;

/**
 * Canvas component (root of UI scene)
 */
typedef struct
{
} ui_ECS_CanvasComponent;

typedef struct
{
  ui_LayoutType type;
  ui_LayoutPadding margins;
  uint16_t spacing;
} ui_ECS_LayoutComponent;
