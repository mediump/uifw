#include "Canvas.hpp"

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/Layout/LayoutTypes.hpp"

using namespace ui;

Canvas ui::createCanvas(const ecs::ECSRoot *root,
                        const uint16_t x,
                        const uint16_t y,
                        const uint16_t width,
                        const uint16_t height,
                        const char *name)
{
  const ecs::Entity entity = ecs::createEntity(root, x, y, width, height, name);

  entity.set<LayoutComponent>({
    .type = LayoutType_Vertical,
    .margins = {
      .top = 0,
      .right = 0,
      .bottom = 0,
      .left = 0,
    },
    .spacing = 0
  });

  return Canvas{entity};
}