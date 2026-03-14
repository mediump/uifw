#include "Canvas.h"

#include "uifw/ECS/Components.h"
#include "uifw/Platform/Window.h"

ui_ECS_Entity ui_ECS_createCanvas(ui_LayoutType layoutType,
                                  ui_LayoutPadding margins,
                                  uint16_t spacing,
                                  ui_ECS_World *world,
                                  const ui_Window *window)
{
  ECS_COMPONENT(world, ui_ECS_LayoutComponent);

  uint16_t windowWidth, windowHeight;
  ui_getWindowSize(window, &windowWidth, &windowHeight);

  const ui_Rect rect = {
    0,
    0,
    windowWidth,
    windowHeight,
  };

  const ui_ECS_Entity entity = ui_ECS_createEntity(world, rect, "Canvas", UI_NULL_ENTITY);

  ecs_set(world, entity, ui_ECS_LayoutComponent, {
    .type = layoutType,
    .margins = margins,
    .spacing = spacing,
  });

  return entity;
}
