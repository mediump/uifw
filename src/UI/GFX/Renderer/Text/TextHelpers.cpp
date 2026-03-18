#include "TextHelpers.hpp"

using namespace ui;

ecs::Entity TextHelpers::createTextEntity(const ecs::ECSRoot *root,
                                          FontData *fontData,
                                          const char *text,
                                          const Color4f color,
                                          const uint16_t pixelSize,
                                          const uint16_t x,
                                          const uint16_t y,
                                          const uint16_t width,
                                          const uint16_t height,
                                          const char *name,
                                          const ecs::Entity *parent)
{
  const ecs::Entity entity = ecs::createEntity(root, x, y, width, height, name, parent);

  entity.set<TextComponent>({.text = text,
                             .font = fontData,
                             .color = color,
                             .pixelSize = pixelSize,
                             .horizontalAlignment = TextHAlignment_Left,
                             .verticalAlignment = TextVAlignment_Top,
                             .needsUpdate = true});

  return entity;
}
