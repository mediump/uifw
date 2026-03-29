#include "ScrollArea.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

using namespace ui;

#define SCROLLBAR_WIDTH 16;

void ScrollArea::addScrollbarElement(const ecs::ECSRoot *root,
                                     const ecs::Entity &entity,
                                     TextComponent &textComponent,
                                     const ecs::BaseComponent &base)
{
  const auto name = std::string(entity.name()) + "_Scrollbar";

  auto scrollbarEntity = ecs::createEntity(root, 0, 0, 0, 0, name.c_str(), &entity);
  scrollbarEntity.set<ecs::QuadRendererComponent>(
    {.color = {0.9f, 0.9f, 0.9f, 1.0f}, .borderRadius = {8, 8, 8, 8}});

  textComponent.scrollbar = scrollbarEntity;
  layoutScrollbar(textComponent, base);
}

void ScrollArea::layoutScrollbar(TextComponent &textComponent,
                                 const ecs::BaseComponent &base)
{
  const auto &scrollbar = textComponent.scrollbar;

  if (scrollbar == UI_NULL_ENTITY) {
    return;
  }

  const uint16_t x = base.rect.x + base.rect.width - SCROLLBAR_WIDTH;
  const uint16_t y = base.rect.y;
  const uint16_t w = SCROLLBAR_WIDTH;
  const uint16_t h = base.rect.height;

  auto scrollbarBase = textComponent.scrollbar.get_ref<ecs::BaseComponent>();
  scrollbarBase->rect = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
  };
}
