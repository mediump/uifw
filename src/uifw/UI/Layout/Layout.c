#include "Layout.h"

#include "uifw/Core/Containers/Queue.h"
#include "uifw/Core/Utils/Log.h"
#include "uifw/ECS/Components.h"

static void clamp_value(uint16_t *value, const uint16_t min, const uint16_t max)
{
  if (*value < min) {
    *value = min;
  }
  else if (*value > max) {
    *value = max;
  }
}

static void layout_children(const ui_ECS_Entity *entity,
                            ui_ECS_World *world,
                            ui_Queue *queue,
                            uint16_t nIteration)
{
  ECS_COMPONENT(world, ui_ECS_BaseComponent);
  ECS_COMPONENT(world, ui_ECS_TransformRelComponent);
  ECS_COMPONENT(world, ui_ECS_LayoutComponent);

  if (!ecs_has(world, *entity, ui_ECS_BaseComponent)) {
    return;
  }

  const auto parentBaseComponent = ecs_get(world, *entity, ui_ECS_BaseComponent);
  const auto parentTransformRel = ecs_get(world, *entity, ui_ECS_TransformRelComponent);
  const auto parentLayoutComponent = ecs_get(world, *entity, ui_ECS_LayoutComponent);

  const size_t nChildren = parentTransformRel->nChildren;

  if (nChildren < 1) {
    return;
  }

  const ui_LayoutType layoutType = parentLayoutComponent->type;
  const ui_LayoutPadding margins = parentLayoutComponent->margins;
  const uint16_t spacing = parentLayoutComponent->spacing;

  ui_ECS_BaseComponent *children[nChildren];

  auto currentRel =
    ecs_get_mut(world, parentTransformRel->first, ui_ECS_TransformRelComponent);

  for (size_t i = 0; i < nChildren; ++i) {
    const auto currentEntity = ecs_get_entity(currentRel);
    children[i] = ecs_get_mut(world, currentEntity, ui_ECS_BaseComponent);

    if (ecs_has(world, *entity, ui_ECS_LayoutComponent)) {
      ui_queuePush(queue, &currentEntity);
    }

    if (currentRel->next == UI_NULL_ENTITY) {
      break;
    }

    currentRel = ecs_get_mut(world, currentRel->next, ui_ECS_TransformRelComponent);
  }

  // Find min/max constraints for current layout axis
  uint16_t minConstraints[nChildren];
  uint16_t maxConstraints[nChildren];

  for (size_t i = 0; i < nChildren; ++i) {
    switch (layoutType) {
    case LayoutType_Horizontal:
      minConstraints[i] = children[i]->minWidth;
      maxConstraints[i] = children[i]->maxWidth;
      break;
    case LayoutType_Vertical:
      minConstraints[i] = children[i]->minHeight;
      maxConstraints[i] = children[i]->maxHeight;
      break;
    }
  }

  // Fit components to available space
  uint16_t availableSpace = 0;
  uint16_t currentPosition = 0;

  switch (layoutType) {
  case LayoutType_Horizontal:
    availableSpace = parentBaseComponent->coords.w - margins.left - margins.right -
      spacing * (nChildren - 1);
    currentPosition = parentBaseComponent->coords.x + margins.left;
    break;
  case LayoutType_Vertical:
    availableSpace = parentBaseComponent->coords.h - margins.top - margins.bottom -
      spacing * (nChildren - 1);
    currentPosition = parentBaseComponent->coords.y + margins.top;
    break;
  default:
    break;
  }

  const uint16_t initialSpace = availableSpace;
  uint16_t computedSizes[nChildren];
  size_t nRemainingComponents = nChildren;

  // Calculate fixed-sized components first
  for (size_t i = 0; i < nChildren; ++i) {
    const uint16_t minSize = minConstraints[i];
    const uint16_t maxSize = maxConstraints[i];

    if (minSize == 0 && maxSize == UINT16_MAX) {
      // Min/max size not set, skip entity in this pass
      continue;
    }

    // Fixed size: use minSize as the target (min and max are set to the same value)
    uint16_t fixedSize = minSize;
    clamp_value(&fixedSize, minSize, maxSize);

    computedSizes[i] = fixedSize;
    availableSpace -= fixedSize;

    nRemainingComponents--;
  }

  // Then calculate flexible entities
  for (size_t i = 0; i < nChildren; ++i) {
    const uint16_t minSize = minConstraints[i];
    const uint16_t maxSize = maxConstraints[i];

    if (minSize > 0 || maxSize < UINT16_MAX) {
      // Min/max size value specified, already handled in first pass
      continue;
    }

    if (nRemainingComponents == 0) {
      computedSizes[i] = 0;
      continue;
    }

    uint16_t inferredSize = availableSpace / nRemainingComponents;
    clamp_value(&inferredSize, minSize, maxSize);

    computedSizes[i] = inferredSize;
    availableSpace -= inferredSize;

    nRemainingComponents--;
  }

  // Calculate secondary axis
  uint16_t secondaryAxisSize = 0;
  uint16_t secondaryAxisPosition = 0;

  switch (layoutType) {
  case LayoutType_Horizontal:
    secondaryAxisSize = parentBaseComponent->coords.h - margins.top - margins.bottom;
    secondaryAxisPosition = parentBaseComponent->coords.y + margins.top;
    break;
  case LayoutType_Vertical:
    secondaryAxisSize = parentBaseComponent->coords.w - margins.left - margins.right;
    secondaryAxisPosition = parentBaseComponent->coords.x + margins.left;
    break;
  }

  // Set sizes
  for (size_t i = 0; i < nChildren; ++i) {
    switch (layoutType) {
    case LayoutType_Horizontal:
      children[i]->coords.x = currentPosition;
      children[i]->coords.y = secondaryAxisPosition;
      children[i]->coords.w = computedSizes[i];
      children[i]->coords.h = secondaryAxisSize;
      break;
    case LayoutType_Vertical:
      children[i]->coords.x = secondaryAxisPosition;
      children[i]->coords.y = currentPosition;
      children[i]->coords.w = secondaryAxisSize;
      children[i]->coords.h = computedSizes[i];
      break;
    default:
      break;
    }
    children[i]->needsUpdate = false;
    currentPosition += computedSizes[i] + spacing;
  }

  // Set z order
  for (size_t i = 0; i < nChildren; ++i) {
    children[i]->zOrder = nIteration;
  }
}

void ui_traverseAndApplyLayout(ui_ECS_World *world, const ui_ECS_Entity *root)
{
#if defined(UIFW_DEBUG)
  ECS_COMPONENT(world, ui_ECS_BaseComponent);
  ECS_COMPONENT(world, ui_ECS_TransformRelComponent);
  ECS_COMPONENT(world, ui_ECS_LayoutComponent);

  const bool hasBase = ecs_has(world, *root, ui_ECS_BaseComponent);
  const bool hasTransformRel = ecs_has(world, *root, ui_ECS_TransformRelComponent);
  const bool hasLayout = ecs_has(world, *root, ui_ECS_LayoutComponent);

  ui_Assert(hasBase, "ui_traverseAndApplyLayout requires `root` to contain a "
                     "ui_ECS_BaseComponent");

  ui_Assert(hasTransformRel, "ui_traverseAndApplyLayout requires `root` to contain a "
                             "ui_ECS_TransformRelComponent");

  ui_Assert(hasLayout, "ui_traverseAndApplyLayout requires `root` to contain a "
                       "ui_ECS_LayoutComponent");
#endif

  ui_Queue queue;

  ui_queueInit(&queue, sizeof(ui_ECS_Entity));
  ui_queuePush(&queue, root);

  size_t i = 0;

  while (!ui_queueEmpty(&queue)) {
    ui_ECS_Entity currentEntity;
    ui_queuePeek(&queue, &currentEntity);

    layout_children(&currentEntity, world, &queue, i);

    ui_queuePop(&queue, &currentEntity);
    ++i;
  }
}
