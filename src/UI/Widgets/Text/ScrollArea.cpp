#include "ScrollArea.hpp"
#include <cstdint>
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "Utils.hpp"

using namespace ui;

#define SCROLLBAR_WIDTH 16

void ScrollArea::addScrollbarElement(const ecs::ECSRoot *root,
                                     const ecs::Entity &entity,
                                     TextComponent &textComponent,
                                     const ecs::BaseComponent &base)
{
  const auto bgName = std::string(entity.name()) + "_Scrollbar";
  const auto handleName = bgName + "_Handle";

  auto background = ecs::createEntity(root, 0, 0, 0, 0, bgName.c_str(), &entity);
  background.set<ecs::QuadRendererComponent>(
    {.color = {0.9f, 0.9f, 0.9f, 1.0f}, .borderRadius = {8, 8, 8, 8}});

  auto handle = ecs::createEntity(root, 0, 0, 0, 0, handleName.c_str(), &background);
  handle.set<ecs::QuadRendererComponent>(
    {.color = {0.5f, 0.5f, 0.5f, 1.0f}, .borderRadius = {6, 6, 6, 6}});

  textComponent.scrollbar = background;
  layoutScrollbar(textComponent, base);
}

void ScrollArea::layoutScrollbar(TextComponent &textComponent,
                                 const ecs::BaseComponent &base)
{
  const auto &background = textComponent.scrollbar;
  const auto &handle = background.get<ecs::BaseComponent>().transformRel.first;

  if (background == UI_NULL_ENTITY || handle == UI_NULL_ENTITY) {
    return;
  }

  layout_background(background, base);
  layout_handle(handle, base);
}

void ScrollArea::layout_background(const ecs::Entity &background,
                                   const ecs::BaseComponent &base)
{
  auto scrollbarBase = background.get_ref<ecs::BaseComponent>();

  const uint16_t x = base.rect.x + base.rect.width - SCROLLBAR_WIDTH;
  const uint16_t y = base.rect.y;
  const uint16_t w = SCROLLBAR_WIDTH;
  const uint16_t h = base.rect.height;

  scrollbarBase->rect = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
  };
}

void ui::ScrollArea::updateScrollbarSize(TextComponent &textComponent,
                                         const ecs::BaseComponent &base,
                                         float textHeight)
{
  if (textComponent.scrollbar == UI_NULL_ENTITY) {
    return;
  }

  const auto background = textComponent.scrollbar;
  auto backgroundBase = background.get_ref<ecs::BaseComponent>();

  const auto handle = backgroundBase->transformRel.first;

  if (handle == UI_NULL_ENTITY) {
    return;
  }

  const uint16_t scrollbarHeight = base.rect.height - 4;

  constexpr uint16_t minHeight = 12;
  const uint16_t maxHeight = scrollbarHeight;

  const float ratio = static_cast<float>(base.rect.height) / textHeight;
  uint16_t height = static_cast<uint16_t>(ratio * scrollbarHeight);

  height = std::max(minHeight, height);
  height = std::min(maxHeight, height);

  auto handleBase = handle.get_ref<ecs::BaseComponent>();
  handleBase->rect.height = height;

  set_scrollbar_visibility(backgroundBase, handleBase, (base.rect.height <= textHeight));
}

void ui::ScrollArea::updateScrollbarPosition(TextComponent &textComponent,
                                             const ecs::BaseComponent &base,
                                             float textHeight)
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
  const float scrollbarTrackHeight = static_cast<float>(base.rect.height - 4);

  const float handleHeight = static_cast<float>(handleBase->rect.height);

  const float maxScrollY = scrollbarTrackHeight - handleHeight;
  const uint16_t y = base.rect.y + 2 + static_cast<uint16_t>(scrollRatio * maxScrollY);

  handleBase->rect.y = y;
}

void ui::ScrollArea::layout_handle(const ecs::Entity &handle,
                                   const ecs::BaseComponent &base)
{
  auto handleBase = handle.get_ref<ecs::BaseComponent>();

  const uint16_t x = base.rect.x + base.rect.width - SCROLLBAR_WIDTH + 2;
  const uint16_t y = base.rect.y + 2;
  const uint16_t w = SCROLLBAR_WIDTH - 4;
  const uint16_t h = base.rect.height - 4;

  handleBase->rect = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
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

  UI_LOG_MSG("Set scrollbar visibility: %s", visible ? "true" : "false");
}
