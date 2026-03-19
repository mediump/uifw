#include "Entity.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"

inline ui::ecs::Entity find_last_child(
    const ui::ecs::TransformRelationship &parentRel)
{
  assert(parentRel.nChildren > 0);

  const auto firstChild = parentRel.first;

  auto currentEntity = firstChild;
  auto currentRel =
      currentEntity.get<ui::ecs::BaseComponent>().transformRel;

  while (currentRel.next != UI_NULL_ENTITY) {
    currentEntity = currentRel.next;
    currentRel =
        currentEntity.get<ui::ecs::BaseComponent>().transformRel;
  }

  return currentEntity;
}

ui::ecs::Entity ui::ecs::createEntity(const ECSRoot *root,
                                      const uint16_t x,
                                      const uint16_t y,
                                      const uint16_t width,
                                      const uint16_t height,
                                      const char *name,
                                      const Entity *parent)
{
  const Entity entity = root->world.entity();

  entity.set_name(name);
  entity.set<BaseComponent>({.rect = {x, y, width, height},
                             .zOrder = 0,
                             .inLayout = false,
                             .needsUpdate = true});

  if (parent != nullptr) {
    auto &parentRel = parent->get_ref<BaseComponent>()->transformRel;
    auto &entityRel = entity.get_ref<BaseComponent>()->transformRel;

    entityRel.parent = *parent;

    if (parentRel.nChildren > 0) {
      // Add child to existing children list
      const auto lastChild = find_last_child(parentRel);
      auto &lastChildRel =
          lastChild.get_ref<BaseComponent>()->transformRel;

      lastChildRel.next = entity;
      entityRel.prev = lastChild;
    } else {
      // Initialize first child
      parentRel.first = entity;
    }

    parentRel.nChildren += 1;
  }

  return entity;
}