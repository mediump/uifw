#pragma once

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

namespace ui {

class ScrollArea
{
public:
  static void addScrollbarElement(const ecs::ECSRoot *root,
                                  const ecs::Entity &entity,
                                  TextComponent &textComponent,
                                  const ecs::BaseComponent &base,
                                  const Vector4i &offsets);

  static void layoutScrollbar(TextComponent &textComponent,
                              const ecs::BaseComponent &base,
                              const Vector4i &offsets);

  static float updateScrollbarSize(TextComponent &textComponent,
                                   const ecs::BaseComponent &base,
                                   float textHeight,
                                   const Vector4i &offsets);

  static void updateScrollbarPosition(TextComponent &textComponent,
                                      const ecs::BaseComponent &base,
                                      float textHeight,
                                      const Vector4i &offsets);

  static bool updateScrollbarInput(TextComponent &textComponent,
                                   const ecs::BaseComponent &base,
                                   const Vector2i &mousePos,
                                   const bool &mouseDown,
                                   const bool &mouseUp,
                                   CursorShape *cursorShape);

  [[nodiscard]] static uint16_t getScrollbarWidth();

private:
  static Rect layout_background(const ecs::Entity &background,
                                const ecs::BaseComponent &base,
                                const Vector4i &offsets);

  static void layout_handle(UI_REF(ecs::BaseComponent) handleBase,
                            const Rect &backgroundBounds);

  static void set_scrollbar_visibility(UI_REF(ecs::BaseComponent) backgroundBase,
                                       UI_REF(ecs::BaseComponent) handleBase,
                                       bool visible);
};

} // namespace ui
