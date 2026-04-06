#pragma once

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/ECSRoot/ECSRoot.hpp"
#include "UI/ECS/Entity/Entity.hpp"

#include <SDL3/SDL_keycode.h>

#include <cstdint>
#include <string>

struct SDL_Window;

namespace ui {

class InputField
{
public:
  static void ensureElements(const ecs::ECSRoot *root,
                             ecs::InputFieldComponent &inputField,
                             const ecs::Entity &entity);

  static size_t getCursorPositionFromMouse(const ecs::InputFieldComponent &input,
                                           const ecs::BaseComponent &base,
                                           const Vector2i &mousePos);

  static void clearSelection(ecs::InputFieldComponent &input);

  static void handleMouseInteraction(const ecs::BaseComponent &base,
                                     ecs::InputFieldComponent &input,
                                     ecs::HoverHandlerComponent &hover,
                                     const Vector2i &mousePos,
                                     bool mouseDown,
                                     bool mouseUp,
                                     SDL_Window *window);

  static bool handleKeyboardInput(ecs::InputFieldComponent &input,
                                  TextComponent *textComp,
                                  const std::string &inputBuffer,
                                  bool keyDown,
                                  SDL_Keycode keyCode,
                                  bool modCtrl);

  static void updateCaret(ecs::InputFieldComponent &input,
                          ecs::BaseComponent *caretBase,
                          const ecs::BaseComponent &fieldBase,
                          const ecs::QuadRendererComponent &quadRenderer,
                          const TextComponent *textComp,
                          uint64_t currentTime);

  static void updateSelectionDrag(ecs::InputFieldComponent &input,
                                  const ecs::BaseComponent &fieldBase,
                                  const Vector2i &mousePos,
                                  bool mouseMoved,
                                  HoverState hoverState);

  static void updateSelectionRect(ecs::InputFieldComponent &input,
                                  ecs::BaseComponent *selectionBase,
                                  const ecs::BaseComponent &fieldBase,
                                  const ecs::QuadRendererComponent &quadRenderer,
                                  const TextComponent *textComp);
};

} // namespace ui
