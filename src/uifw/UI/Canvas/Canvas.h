#pragma once

#include "uifw/Core/Types.h"
#include "uifw/ECS/Entity.h"
#include "uifw/Core/Coordinates.h"

ui_ECS_Entity ui_ECS_createCanvas(ui_LayoutType layoutType,
                                  ui_LayoutPadding margins,
                                  uint16_t spacing,
                                  ui_ECS_World *world,
                                  const ui_Window *window);