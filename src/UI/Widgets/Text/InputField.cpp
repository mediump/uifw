#include "InputField.hpp"

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/Text/TextRendererHelpers.hpp"

#include <algorithm>

using namespace ui;

#define UI_strcat(a, b) std::string(a) + b;

void InputField::ensureElements(const ecs::ECSRoot *root,
                                ecs::InputFieldComponent &inputField,
                                const ecs::Entity &entity)
{
  if (inputField.text == UI_NULL_ENTITY) {
    const auto textName = UI_strcat(entity.name().c_str(), "_text");
    const auto inputFieldText =
      ui::ecs::createEntity(root, 0, 0, 200, 200, textName.c_str(), &entity);

    inputFieldText.set<TextComponent>({.text = "",
                                       .font = inputField.font,
                                       .color = {0.94f, 0.94f, 0.94f, 1.0f},
                                       .pixelSize = 16,
                                       .horizontalAlignment = ui::TextHAlignment_Left,
                                       .verticalAlignment = ui::TextVAlignment_Middle});

    inputField.text = inputFieldText;
  }

  if (inputField.caret == UI_NULL_ENTITY) {
    const auto caretName = UI_strcat(entity.name().c_str(), "_caret");
    const auto caret =
      ui::ecs::createEntity(root, 0, 0, 200, 200, caretName.c_str(), &entity);

    caret.set<ecs::QuadRendererComponent>({.color = {1.0f, 1.0f, 1.0f, 1.0f}});

    inputField.caret = caret;
  }
}

size_t InputField::getCursorPositionFromMouse(const ecs::InputFieldComponent &input,
                                              const ecs::BaseComponent &base,
                                              const Vector2i &mousePos)
{
  if (input.text != UI_NULL_ENTITY) {
    const auto &inputText = input.text.get<ui::TextComponent>();

    if (inputText.text.empty()) {
      return 0;
    }

    const std::vector<float> glyphDimensions =
      TextUtils::computeLineWidth(inputText.text, inputText, inputText.font);

    float totalAdvance = 0.0f;
    std::vector<float> absoluteMidpoints;
    absoluteMidpoints.reserve(glyphDimensions.size());

    for (const auto &width : glyphDimensions) {
      const float min = base.rect.x + totalAdvance;
      const float max = min + width;

      absoluteMidpoints.emplace_back((min + max) * 0.5f);
      totalAdvance += width;
    }

    if (absoluteMidpoints.back() < mousePos.x) {
      return inputText.text.size();
    }

    float minDist = std::numeric_limits<float>::max();
    size_t minIdx = 0;

    for (size_t i = 0; i < absoluteMidpoints.size(); i++) {
      const float dist = std::abs(mousePos.x - absoluteMidpoints[i]);

      if (dist < minDist) {
        minDist = dist;
        minIdx = i;
      }
    }

    return std::clamp<size_t>(minIdx, 0, inputText.text.size());
  }
}
