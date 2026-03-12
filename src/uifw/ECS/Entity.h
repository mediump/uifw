#pragma once

#include "World.h"

#include "uifw/Core/Coordinates.h"

typedef ecs_entity_t ui_ECS_Entity;

#define UI_NULL_ENTITY 0

#define UI_ECS_GET_REF(world, entity, T, out)                  \
  do {                                                         \
    ecs_ref_t ref = ecs_ref_init_id(world, entity, ecs_id(T)); \
    out = ecs_ref_get(world, &ref, ecs_id(T));                 \
  }                                                            \
  while (0)

ui_ECS_Entity ui_ECS_createEntity(ui_ECS_World *world,
                                  ui_Rect coords,
                                  const char *name,
                                  ui_ECS_Entity parent);