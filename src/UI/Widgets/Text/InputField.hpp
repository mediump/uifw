#pragma once

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/ECSRoot/ECSRoot.hpp"
#include "UI/ECS/Entity/Entity.hpp"

namespace ui {

class InputField
{
public:
  static void ensureElements(const ecs::ECSRoot *root,
                             ecs::InputFieldComponent &inputField,
                             const ecs::Entity &entity);
};

} // namespace ui
