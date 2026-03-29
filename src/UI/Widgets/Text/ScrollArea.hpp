#pragma once

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

namespace ui {

class ScrollArea
{
public:
  static void addScrollbarElement(const ecs::ECSRoot *root,
                                  const ecs::Entity &entity,
                                  TextComponent &textComponent,
                                  const ecs::BaseComponent &base);

  static void layoutScrollbar(TextComponent &textComponent,
                              const ecs::BaseComponent &base);

private:
  static void layout_background(const ecs::Entity &background,
                                const ecs::BaseComponent &base);

  static void layout_handle(const ecs::Entity &handle, 
    const ecs::BaseComponent &base);
};

} // namespace ui
