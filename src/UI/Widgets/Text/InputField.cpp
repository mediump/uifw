#include "InputField.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

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

    caret.set<ecs::QuadRendererComponent>({
      .color = {1.0f, 1.0f, 1.0f, 1.0f}
    });

    inputField.caret = caret;
  }
}
