#pragma once

#include "UI/ECS/Entity/Entity.hpp"

#include <queue>

namespace ui {

class Layout
{
public:
  /**
   * Will start at the root and traverse all child entities in breadth-first
   * order. Practically this should be used each time something significant
   * in the UI stack has changed, such as a window resize. All child entities
   * will be resized with respect to their constraints
   *
   * @param rootEntity The root element to begin the traversal from
   */
  static void traverseAndApplyLayout(const ecs::Entity &rootEntity);

private:
  static void layout_children(const ecs::Entity &parent,
                              std::queue<ecs::Entity> &entityQueue);
};

}  // namespace ui
