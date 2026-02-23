#pragma once

#include "UI/ECS/ECSRoot/ECSRoot.hpp"
#include "UI/ECS/Entity/Entity.hpp"

namespace ui {

struct Canvas
{
  ecs::Entity entity;
};

Canvas createCanvas(const ecs::ECSRoot *root,
                    uint16_t x,
                    uint16_t y,
                    uint16_t width,
                    uint16_t height,
                    const char *name);

}  // namespace ui
