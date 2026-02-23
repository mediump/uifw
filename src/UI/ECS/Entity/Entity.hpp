#pragma once

#include "UI/ECS/ECSRoot/ECSRoot.hpp"

#include <flecs.h>

#define UI_NULL_ENTITY flecs::entity::null()
#define UI_REF(T) flecs::ref<T>

namespace ui::ecs {

typedef flecs::entity Entity;

Entity createEntity(const ECSRoot *root,
                    uint16_t x,
                    uint16_t y,
                    uint16_t width = 128.0f,
                    uint16_t height = 128.0f,
                    const char *name = "",
                    const Entity *parent = nullptr);

}  // namespace ui::ecs
