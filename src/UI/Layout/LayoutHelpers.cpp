#include "LayoutHelpers.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"

#include <vector>

#define UI_SIZE_UNSET -1.0f;

using namespace ui;

void Layout::traverseAndApplyLayout(const ecs::Entity &rootEntity)
{
  std::queue<ecs::Entity> queue;
  queue.push(rootEntity);

  while (!queue.empty()) {
    const ecs::Entity currentEntity = queue.front();
    layout_children(currentEntity, queue);
    queue.pop();
  }
}

void Layout::layout_children(const ecs::Entity &parent,
                             std::queue<ecs::Entity> &entityQueue)
{
  if (!parent.has<LayoutComponent>()) {
    return;
  }

  const auto parentLayoutComponent = parent.get<LayoutComponent>();

  const LayoutType layoutType = parentLayoutComponent.type;

  if (layoutType == LayoutType_None) {
    return;
  }

  const auto parentBaseComponent = parent.get<ecs::BaseComponent>();

  const size_t nChildren = parentBaseComponent.transformRel.nChildren;

  if (nChildren == 0) {
    return;
  }

  const LayoutMargins margins = parentLayoutComponent.margins;
  const uint16_t spacing = parentLayoutComponent.spacing;

  std::vector<UI_REF(ecs::BaseComponent)> children(nChildren);
  auto currentComponent =
      parentBaseComponent.transformRel.first.get_ref<ecs::BaseComponent>();

  for (size_t i = 0; i < nChildren; ++i) {
    const auto currentEntity = currentComponent.entity();
    children[i] = currentComponent;

    if (currentEntity.has<LayoutComponent>()) {
      entityQueue.push(currentEntity);
    }

    if (currentComponent->transformRel.next == UI_NULL_ENTITY) {
      break;
    }

    currentComponent =
        currentComponent->transformRel.next.get_ref<ecs::BaseComponent>();
  }

  // Pair of min/max values for the layout axis
  std::vector<std::pair<uint16_t, uint16_t>> inSizeConstraints(nChildren);

  for (size_t i = 0; i < nChildren; ++i) {
    switch (layoutType) {
      case LayoutType_Horizontal:
        inSizeConstraints[i] =
            std::make_pair(children[i]->minWidth, children[i]->maxWidth);
        break;
      case LayoutType_Vertical:
        inSizeConstraints[i] =
            std::make_pair(children[i]->minHeight, children[i]->maxHeight);
        break;
      default:
        break;
    }
  }

  // Fit components to available space
  uint16_t availableSpace = 0;
  uint16_t currentPosition = 0;

  switch (layoutType) {
    case LayoutType_Horizontal:
      availableSpace = parentBaseComponent.rect.width - margins.left -
                       margins.right - spacing * (nChildren - 1);

      currentPosition = parentBaseComponent.rect.x + margins.left;
      break;
    case LayoutType_Vertical:
      availableSpace = parentBaseComponent.rect.height - margins.top -
                       margins.bottom - spacing * (nChildren - 1);

      currentPosition = parentBaseComponent.rect.y + margins.top;
      break;
    default:
      break;
  }

  const uint16_t initialSpace = availableSpace;
  std::vector<uint16_t> computedSizes(nChildren);
  size_t nRemainingComponents = nChildren;

  // Calculate fixed-sized components first
  for (size_t i = 0; i < nChildren; ++i) {
    const uint16_t minSize = inSizeConstraints[i].first;
    const uint16_t maxSize = inSizeConstraints[i].second;

    if (minSize == 0 && maxSize == std::numeric_limits<uint16_t>::max()) {
      // Min/max size not set, skip entity
      continue;
    }

    uint16_t inferredSize = availableSpace / nRemainingComponents;
    clamp_value(&inferredSize, minSize, maxSize);

    computedSizes[i] = inferredSize;
    availableSpace -= inferredSize;

    nRemainingComponents--;
  }

  // Then calculate everything else
  for (size_t i = 0; i < nChildren; ++i) {
    const uint16_t minSize = inSizeConstraints[i].first;
    const uint16_t maxSize = inSizeConstraints[i].second;

    if (minSize > 0 && maxSize < std::numeric_limits<uint16_t>::max()) {
      // Min/max size value specified, skip entity
      continue;
    }

    uint16_t inferredSize = availableSpace / nRemainingComponents;
    clamp_value(&inferredSize, minSize, maxSize);

    computedSizes[i] = inferredSize;
    availableSpace -= inferredSize;

    nRemainingComponents--;
  }

  uint16_t secondaryAxisSize = 0.0f;
  uint16_t secondaryAxisPosition = 0.0f;

  switch (layoutType) {
    case LayoutType_Horizontal:
      secondaryAxisSize =
          parentBaseComponent.rect.height - margins.top - margins.bottom;
      secondaryAxisPosition = parentBaseComponent.rect.y + margins.top;
      break;
    case LayoutType_Vertical:
      secondaryAxisSize =
          parentBaseComponent.rect.width - margins.left - margins.right;
      secondaryAxisPosition = parentBaseComponent.rect.x + margins.left;
      break;
    default:
      break;
  }

  // Set sizes
  for (size_t i = 0; i < nChildren; ++i) {
    switch (layoutType) {
      case LayoutType_Horizontal:
        children[i]->rect = {currentPosition, secondaryAxisPosition,
                             computedSizes[i], secondaryAxisSize};
        break;
      case LayoutType_Vertical:
        children[i]->rect = {secondaryAxisPosition, currentPosition,
                             secondaryAxisSize, computedSizes[i]};
        break;
      default:
        break;
    }
    children[i]->needsUpdate = false;
    currentPosition += computedSizes[i] + spacing;
  }
}

void Layout::clamp_value(uint16_t *value,
                         const uint16_t min,
                         const uint16_t max)
{
  if (value == nullptr) {
    return;
  }

  if (*value < min) {
    *value = min;
  }

  if (*value > max) {
    *value = max;
  }
}