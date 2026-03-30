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

  static float updateScrollbarSize(TextComponent &textComponent,
                                   const ecs::BaseComponent &base,
                                   float textHeight);

  static void updateScrollbarPosition(TextComponent &textComponent,
                                      const ecs::BaseComponent &base,
                                      float textHeight);

  static bool updateScrollbarInput(TextComponent &textComponent,
                                   const ecs::BaseComponent &base,
                                   const Vector2i &mousePos,
                                   const bool &mouseDown,
                                   const bool &mouseUp);

  [[nodiscard]] static uint16_t getScrollbarWidth();

private:
  static void layout_background(const ecs::Entity &background,
                                const ecs::BaseComponent &base);

  static void layout_handle(const ecs::Entity &handle, const ecs::BaseComponent &base);

  static void set_scrollbar_visibility(UI_REF(ecs::BaseComponent) backgroundBase,
                                       UI_REF(ecs::BaseComponent) handleBase,
                                       bool visible);
};

} // namespace ui
