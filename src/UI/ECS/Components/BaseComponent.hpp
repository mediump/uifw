#pragma once

#include "UI/Layout/LayoutTypes.hpp"
#include "UI/ECS/Entity/Entity.hpp"

namespace ui::ecs {

struct TransformRelationship
{
  size_t nChildren = 0;
  Entity first;
  Entity prev;
  Entity next;
  Entity parent;
};

struct BaseComponent {
  TransformRelationship transformRel = {};
  Rect rect = { 0, 0, 128, 128 };

  uint16_t minWidth = 0;
  uint16_t minHeight = 0;
  uint16_t maxWidth = std::numeric_limits<uint16_t>::max();
  uint16_t maxHeight = std::numeric_limits<uint16_t>::max();

  uint16_t zOrder = 0;
  uint16_t inLayout = false;
  uint16_t needsUpdate = true;
};

}