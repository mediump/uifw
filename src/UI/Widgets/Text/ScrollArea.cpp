#include "ScrollArea.hpp"

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/RendererTypes.hpp"
#include "UI/IO/Input/InputHelpers.hpp"

#include <cstdint>

using namespace ui;

#define SCROLLBAR_WIDTH   16
#define SCROLLBAR_PADDING 4
#define SCROLLBAR_HOVER_GROW 1

#define BORDER_TOP(b)     b.z
#define BORDER_BOTTOM(b)  b.x
#define BORDER_LEFT(b)    b.w
#define BORDER_RIGHT(b)   b.y

void ScrollArea::addScrollbarElement(const ecs::ECSRoot *root,
                                     const ecs::Entity &entity,
                                     TextComponent &textComponent,
                                     const ecs::BaseComponent &base,
                                     const Vector4i &offsets)
{
  const auto bgName = std::string(entity.name()) + "_Scrollbar";
  const auto handleName = bgName + "_Handle";

  auto background = ecs::createEntity(root, 0, 0, 0, 0, bgName.c_str(), &entity);

  auto backgroundBase = background.get_ref<ecs::BaseComponent>();
  backgroundBase->zOrder = 5;

  background.set<ecs::QuadRendererComponent>(
    {.color = {0.9f, 0.9f, 0.9f, 0.0f}, .borderRadius = {0, 0, 0, 0}});

  auto handle = ecs::createEntity(root, 0, 0, 0, 0, handleName.c_str(), &background);

  auto handleBase = handle.get_ref<ecs::BaseComponent>();
  handleBase->zOrder = 10;

  handle.set<ecs::QuadRendererComponent>(
    {.color = {0.5f, 0.5f, 0.5f, 1.0f}, .borderRadius = {5, 5, 5, 5}});

  handle.set<ecs::HoverHandlerComponent>({
    .cursorShape = CursorShape_Pointer,
    .state = HoverState_Idle
  });

  textComponent.scrollbar = background;
  layoutScrollbar(textComponent, base, offsets);
}

void ScrollArea::layoutScrollbar(TextComponent &textComponent,
                                 const ecs::BaseComponent &base,
                                 const Vector4i &offsets)
{
  const auto &background = textComponent.scrollbar;
  const auto &handle = background.get<ecs::BaseComponent>().transformRel.first;

  if (background == UI_NULL_ENTITY || handle == UI_NULL_ENTITY) {
    return;
  }

  auto handleBase = handle.get_ref<ecs::BaseComponent>();
  const Rect backgroundBounds = layout_background(background, base, offsets);

  layout_handle(handleBase, backgroundBounds);
}

Rect ScrollArea::layout_background(const ecs::Entity &background,
                                   const ecs::BaseComponent &base,
                                   const Vector4i &offsets)
{
  auto scrollbarBase = background.get_ref<ecs::BaseComponent>();

  const uint16_t x =
    base.rect.x + base.rect.width - SCROLLBAR_WIDTH - BORDER_RIGHT(offsets);
  const uint16_t y = base.rect.y + BORDER_TOP(offsets);
  const uint16_t w = SCROLLBAR_WIDTH;
  const uint16_t h =
    std::abs(base.rect.height - BORDER_TOP(offsets) - BORDER_BOTTOM(offsets));

  const Rect result = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
  };

  scrollbarBase->rect = result;

  return result;
}

float ui::ScrollArea::updateScrollbarSize(TextComponent &textComponent,
                                          const ecs::BaseComponent &base,
                                          float textHeight,
                                          const Vector4i &offsets)
{
  if (textComponent.scrollbar == UI_NULL_ENTITY) {
    return 0.0f;
  }

  const auto background = textComponent.scrollbar;
  auto backgroundBase = background.get_ref<ecs::BaseComponent>();

  const auto handle = backgroundBase->transformRel.first;

  if (handle == UI_NULL_ENTITY) {
    return 0.0f;
  }

  const uint16_t scrollbarHeight =
    base.rect.height - BORDER_TOP(offsets) - BORDER_BOTTOM(offsets);

  constexpr uint16_t minHeight = 12;
  const uint16_t maxHeight = scrollbarHeight;

  const float ratio = static_cast<float>(base.rect.height) / textHeight;
  uint16_t height = static_cast<uint16_t>(ratio * scrollbarHeight);

  height = std::max(minHeight, height);
  height = std::min(maxHeight, height);

  auto handleBase = handle.get_ref<ecs::BaseComponent>();
  handleBase->rect.height = height;

  set_scrollbar_visibility(backgroundBase, handleBase, (base.rect.height <= textHeight));
  return height;
}

void ui::ScrollArea::updateScrollbarPosition(TextComponent &textComponent,
                                             const ecs::BaseComponent &base,
                                             float textHeight,
                                             const Vector4i &offsets)
{
  if (textComponent.scrollbar == UI_NULL_ENTITY) {
    return;
  }

  const auto background = textComponent.scrollbar;
  const auto handle = background.get<ecs::BaseComponent>().transformRel.first;

  if (handle == UI_NULL_ENTITY) {
    return;
  }

  auto handleBase = handle.get_ref<ecs::BaseComponent>();

  const float scrollableHeight = textHeight - base.rect.height;
  if (scrollableHeight <= 0.0f) {
    return;
  }

  const float scrollRatio = textComponent.scrollPosition / -scrollableHeight;

  const float scrollbarTrackHeight =
    static_cast<float>(background.get<ecs::BaseComponent>().rect.height - 4);
  const float handleHeight = static_cast<float>(handleBase->rect.height);

  const float maxScrollY = scrollbarTrackHeight - handleHeight;
  const uint16_t y = background.get<ecs::BaseComponent>().rect.y + 2 +
    static_cast<uint16_t>(scrollRatio * maxScrollY);

  handleBase->rect.y = y;
}

bool ui::ScrollArea::updateScrollbarInput(TextComponent &textComponent,
                                          const ecs::BaseComponent &base,
                                          const Vector2i &mousePos,
                                          const bool &mouseDown,
                                          const bool &mouseUp,
                                          CursorShape* cursorShape)
{
  bool isClicked = false;

  if (textComponent.scrollbar != UI_NULL_ENTITY) {
    const auto background = textComponent.scrollbar.get<ecs::BaseComponent>();

    bool hovered = false;
    bool clicked = false;

    if (background.transformRel.nChildren > 0) {
      const auto handle = background.transformRel.first;
      const auto handleBase = handle.get<ecs::BaseComponent>();

      if (handle.has<ecs::QuadRendererComponent>() &&
          handle.has<ecs::HoverHandlerComponent>()) {
        auto handleQuadRenderer = handle.get_ref<ecs::QuadRendererComponent>();
        auto handleHoverHandler = handle.get_ref<ecs::HoverHandlerComponent>();

        constexpr ecs::Color idleColor = {0.5f, 0.5f, 0.5f, 0.75f};
        constexpr ecs::Color hoveredColor = {0.5f, 0.5f, 0.5f, 0.8f};
        constexpr ecs::Color clickedColor = {0.55f, 0.55f, 0.55f, 1.0f};

        // Detect hover state
        if (mouseUp && handleHoverHandler->state == HoverState_Clicked) {
          handleHoverHandler->state = HoverState_Idle;
        }
        else {
          if (InputHelpers::isMouseInRect(mousePos, handleBase.rect)) {
            if (handleHoverHandler->state != HoverState_Clicked &&
                handleHoverHandler->state != HoverState_Hovered) {
              handleHoverHandler->state = HoverState_Hovered;
            }
            else {
              if (mouseDown) {
                handleHoverHandler->state = HoverState_Clicked;
              }
            }
          }
          else {
            if (handleHoverHandler->state != HoverState_Clicked &&
                handleHoverHandler->state != HoverState_Idle) {
              handleHoverHandler->state = HoverState_Idle;
            }
          }
        }

        // Apply style
        switch (handleHoverHandler->state) {
        case HoverState_Idle:
          if (handleQuadRenderer->color != idleColor) {
            handleQuadRenderer->color = idleColor;
          }
          break;
        case HoverState_Hovered:
          if (handleQuadRenderer->color != hoveredColor) {
            handleQuadRenderer->color = hoveredColor;
          }
          break;
        case HoverState_Clicked:
          if (handleQuadRenderer->color != clickedColor) {
            handleQuadRenderer->color = clickedColor;
          }
          isClicked = true;
          break;
        }

        // Update cursor
        if (handleHoverHandler->state == HoverState_Idle) {
          *cursorShape = CursorShape_Default;
        }
        else {
          *cursorShape = CursorShape_Pointer;
        }
      }
    }
  }

  return isClicked;
}

[[nodiscard]] uint16_t ui::ScrollArea::getScrollbarWidth()
{
  return SCROLLBAR_WIDTH;
}

void ui::ScrollArea::layout_handle(UI_REF(ecs::BaseComponent) handleBase,
                                   const Rect &backgroundBounds)
{
  const auto &scrollbarEntity = handleBase.entity();
  float padding = SCROLLBAR_PADDING;

  if (scrollbarEntity.has<ecs::HoverHandlerComponent>()) {
    const auto &hoverHandler = scrollbarEntity.get<ecs::HoverHandlerComponent>();
    if (hoverHandler.state != HoverState_Idle) {
      padding -= SCROLLBAR_HOVER_GROW;
    }
  }

  const uint16_t x = backgroundBounds.x + padding;
  const uint16_t y = backgroundBounds.y + padding;
  const uint16_t w = backgroundBounds.width - padding * 2;
  const uint16_t h = backgroundBounds.height - padding * 2;

  handleBase->rect = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
  };

  if (scrollbarEntity.has<ecs::QuadRendererComponent>()) {
    auto quadRenderer = scrollbarEntity.get_ref<ecs::QuadRendererComponent>();

    const float r = static_cast<float>(w) * 0.5f;
    quadRenderer->borderRadius = {r, r, r, r};
  };
}

void ui::ScrollArea::set_scrollbar_visibility(UI_REF(ecs::BaseComponent) backgroundBase,
                                              UI_REF(ecs::BaseComponent) handleBase,
                                              bool visible)
{
  if (backgroundBase->visible != visible) {
    backgroundBase->visible = visible;
  }

  if (handleBase->visible != visible) {
    handleBase->visible = visible;
  }
}
