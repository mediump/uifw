#pragma once

#include "Utils.hpp"

#include <flecs.h>

namespace ui::ecs {

struct ECSRoot
{
  // The root of the ECS world. This system uses the flecs library to manage
  // entities
  //
  flecs::world *world;
};

}  // namespace ui::ecs
