#include "Entity.h"

#include "uifw/Core/Utils/Log.h"
#include "uifw/ECS/Components.h"

static inline ui_ECS_Entity find_last_child(ui_ECS_World *world,
                                            const ui_ECS_TransformRelComponent *parentRel)
{
  ECS_COMPONENT(world, ui_ECS_TransformRelComponent);
  ui_Assert(parentRel->nChildren > 0);

  const ui_ECS_Entity firstChild = parentRel->first;

  ui_ECS_Entity currentEntity = firstChild;
  auto currentRel = ecs_get(world, currentEntity, ui_ECS_TransformRelComponent);

  while (currentRel->next != UI_NULL_ENTITY) {
    currentEntity = currentRel->next;
    currentRel = ecs_get(world, currentEntity, ui_ECS_TransformRelComponent);
  }

  return currentEntity;
}

ui_ECS_Entity ui_ECS_createEntity(ui_ECS_World *world,
                                  ui_Rect coords,
                                  const char *name,
                                  ui_ECS_Entity parent)
{
  ECS_COMPONENT(world, ui_ECS_BaseComponent);
  ECS_COMPONENT(world, ui_ECS_TransformRelComponent);

  const ui_ECS_Entity entity = ecs_entity(world, {.name = name});

  ecs_set(world, entity, ui_ECS_BaseComponent,
          {.coords = coords,
           .minWidth = 0,
           .minHeight = 0,
           .maxWidth = UINT16_MAX,
           .maxHeight = UINT16_MAX,
           .zOrder = 0,
           .needsUpdate = false});
  ecs_add(world, entity, ui_ECS_TransformRelComponent);

  // Set parenting
  if (parent != UI_NULL_ENTITY) {
    auto parentRel = ecs_get_mut(world, parent, ui_ECS_TransformRelComponent);
    auto entityRel = ecs_get_mut(world, parent, ui_ECS_TransformRelComponent);

    entityRel->parent = parent;

    if (parentRel->nChildren > 0) {
      // Add child to existing children list
      const ui_ECS_Entity lastChild = find_last_child(world, parentRel);
      ui_ECS_TransformRelComponent *lastChildRel = ecs_get_mut(
        world, lastChild, ui_ECS_TransformRelComponent);

      lastChildRel->next = entity;
      entityRel->prev = lastChild;
    }
    else {
      parentRel->first = entity;
    }

    parentRel->nChildren += 1;
  }

  return entity;
}
