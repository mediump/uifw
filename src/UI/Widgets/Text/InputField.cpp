#include "InputField.hpp"

#include "SDL3/SDL_clipboard.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"

#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/Text/TextRendererHelpers.hpp"

#include <algorithm>
#include <limits>
#include <numeric>

using namespace ui;

#define UI_strcat(a, b) std::string(a) + b;

constexpr uint64_t CARET_BLINK_RATE = 1000;
constexpr uint64_t CARET_VISIBLE_AFTER_INPUT = 300;

#define BORDER_TOP(b)    b.z
#define BORDER_BOTTOM(b) b.x
#define BORDER_LEFT(b)   b.w
#define BORDER_RIGHT(b)  b.y

static bool isMouseInRect(const Vector2i &mousePos, const Rect &rect)
{
  return mousePos.x >= rect.x && mousePos.x <= rect.x + rect.width &&
    mousePos.y >= rect.y && mousePos.y <= rect.y + rect.height;
}

static void deleteSelectedText(ecs::InputFieldComponent &input, TextComponent &textComp)
{
  size_t selMin = std::min(input.selectionStart, input.selectionEnd);
  size_t selMax = std::max(input.selectionStart, input.selectionEnd);
  textComp.text.erase(selMin, selMax - selMin);
  input.cursorPos = selMin;
  InputField::clearSelection(input);
}

void InputField::handleMouseInteraction(const ecs::BaseComponent &base,
                                        ecs::InputFieldComponent &input,
                                        ecs::HoverHandlerComponent &hover,
                                        const Vector2i &mousePos,
                                        bool mouseDown,
                                        bool mouseUp,
                                        ::SDL_Window *window)
{
  if (isMouseInRect(mousePos, base.rect)) {
    if (mouseDown) {
      SDL_StartTextInput(window);
      input.state = ecs::InputFieldState_Active;
      input.lastInputTime = SDL_GetTicks();
      input.cursorPos = InputField::getCursorPositionFromMouse(input, base, mousePos);
      hover.state = HoverState_Clicked;
      InputField::clearSelection(input);
    }
    else if (mouseUp) {
      hover.state = HoverState_Idle;
    }
  }
  else {
    if (mouseDown) {
      SDL_StopTextInput(window);
      input.state = ecs::InputFieldState_Inactive;
    }
    else if (mouseUp) {
      hover.state = HoverState_Idle;
    }
  }
}

bool InputField::handleKeyboardInput(ecs::InputFieldComponent &input,
                                     TextComponent *textComp,
                                     const std::string &inputBuffer,
                                     bool keyDown,
                                     SDL_Keycode keyCode,
                                     bool modCtrl)
{
  bool bufferChanged = false;

  if (!inputBuffer.empty()) {
    if (input.selectionStart != input.selectionEnd) {
      deleteSelectedText(input, *textComp);
    }
    textComp->text.insert(input.cursorPos, inputBuffer);
    input.cursorPos += inputBuffer.size();
    bufferChanged = true;
  }
  else if (keyDown) {
    switch (keyCode) {
    case SDLK_BACKSPACE:
      if (input.selectionStart != input.selectionEnd) {
        deleteSelectedText(input, *textComp);
      }
      else if (!textComp->text.empty() && input.cursorPos > 0) {
        textComp->text.erase(textComp->text.begin() + input.cursorPos - 1);
        input.cursorPos -= 1;
      }
      bufferChanged = true;
      break;

    case SDLK_DELETE:
      if (input.selectionStart != input.selectionEnd) {
        deleteSelectedText(input, *textComp);
      }
      else if (!textComp->text.empty() && input.cursorPos < textComp->text.size()) {
        textComp->text.erase(textComp->text.begin() + input.cursorPos);
      }
      bufferChanged = true;
      break;

    case SDLK_RIGHT:
      if (!textComp->text.empty()) {
        if (input.selectionStart != input.selectionEnd && !modCtrl) {
          input.cursorPos = input.selectionEnd;
          InputField::clearSelection(input);
        }
        else {
          input.cursorPos += 1;
        }
      }
      break;

    case SDLK_LEFT:
      if (!textComp->text.empty()) {
        if (input.selectionStart != input.selectionEnd && !modCtrl) {
          input.cursorPos = input.selectionStart;
          InputField::clearSelection(input);
        }
        else {
          input.cursorPos -= 1;
        }
      }
      break;

    case SDLK_RETURN:
      break;

    case SDLK_A:
      if (modCtrl) {
        input.selectionStart = 0;
        input.selectionEnd = textComp->text.size();
      }
      break;

    case SDLK_X:
      if (modCtrl && input.selectionStart != input.selectionEnd) {
        size_t selMin = std::min(input.selectionStart, input.selectionEnd);
        size_t selMax = std::max(input.selectionStart, input.selectionEnd);
        std::string selectedText = textComp->text.substr(selMin, selMax - selMin);
        SDL_SetClipboardText(selectedText.c_str());
        deleteSelectedText(input, *textComp);
      }
      bufferChanged = true;
      break;

    case SDLK_C:
      if (modCtrl && input.selectionStart != input.selectionEnd) {
        size_t selMin = std::min(input.selectionStart, input.selectionEnd);
        size_t selMax = std::max(input.selectionStart, input.selectionEnd);
        std::string selectedText = textComp->text.substr(selMin, selMax - selMin);
        SDL_SetClipboardText(selectedText.c_str());
      }
      break;

    case SDLK_V:
      if (modCtrl) {
        const char *clipboardText = SDL_GetClipboardText();
        if (clipboardText && SDL_strlen(clipboardText) > 0) {
          if (input.selectionStart != input.selectionEnd) {
            deleteSelectedText(input, *textComp);
          }
          textComp->text.insert(input.cursorPos, clipboardText);
          input.cursorPos += SDL_strlen(clipboardText);
          bufferChanged = true;
        }
      }
      break;

    default:
      break;
    }
  }

  input.cursorPos = std::clamp<size_t>(input.cursorPos, 0, textComp->text.size());
  return bufferChanged;
}

void InputField::updateCaret(ecs::InputFieldComponent &input,
                             ecs::BaseComponent *caretBase,
                             const ecs::BaseComponent &fieldBase,
                             const ecs::QuadRendererComponent &quadRenderer,
                             const TextComponent *textComp,
                             uint64_t currentTime)
{
  if (input.state == ecs::InputFieldState_Active) {
    const std::vector<float> glyphDimensions =
      TextUtils::computeLineWidth(textComp->text, *textComp, textComp->font);

    const uint64_t timeSinceLastInput = currentTime - input.lastInputTime;
    const bool recentlyTyped = timeSinceLastInput < CARET_VISIBLE_AFTER_INPUT;

    if (recentlyTyped) {
      caretBase->visible = true;
    }
    else {
      caretBase->visible = (currentTime % CARET_BLINK_RATE) < (CARET_BLINK_RATE / 2);
    }

    const float leftBorder = BORDER_LEFT(quadRenderer.borderWidths);
    const float topBorder = BORDER_TOP(quadRenderer.borderWidths);

    caretBase->rect.y = static_cast<uint16_t>(fieldBase.rect.y + topBorder + 2);
    caretBase->rect.height = static_cast<uint16_t>(
      fieldBase.rect.height - topBorder - BORDER_BOTTOM(quadRenderer.borderWidths) - 4);

    const float cursorOffset = std::accumulate(
      glyphDimensions.begin(), glyphDimensions.begin() + input.cursorPos, 0.0f);
    caretBase->rect.x =
      static_cast<uint16_t>(fieldBase.rect.x + leftBorder + cursorOffset);
    caretBase->rect.width = 2;
    caretBase->zOrder = 90;
  }
  else {
    caretBase->visible = false;
  }
}

void InputField::updateSelectionDrag(ecs::InputFieldComponent &input,
                                     const ecs::BaseComponent &fieldBase,
                                     const Vector2i &mousePos,
                                     bool mouseMoved,
                                     HoverState hoverState)
{
  if (input.state == ecs::InputFieldState_Active && hoverState == HoverState_Clicked &&
      mouseMoved) {
    input.selectionEnd =
      InputField::getCursorPositionFromMouse(input, fieldBase, mousePos);
  }
}

void InputField::updateSelectionRect(ecs::InputFieldComponent &input,
                                     ecs::BaseComponent *selectionBase,
                                     const ecs::BaseComponent &fieldBase,
                                     const ecs::QuadRendererComponent &quadRenderer,
                                     const TextComponent *textComp)
{
  if (input.selectionStart != input.selectionEnd) {
    const std::vector<float> glyphDimensions =
      TextUtils::computeLineWidth(textComp->text, *textComp, textComp->font);

    selectionBase->visible = true;

    const float leftBorder = BORDER_LEFT(quadRenderer.borderWidths);

    size_t selMin = std::min(input.selectionStart, input.selectionEnd);
    size_t selMax = std::max(input.selectionStart, input.selectionEnd);

    const float selectionOffset =
      std::accumulate(glyphDimensions.begin(), glyphDimensions.begin() + selMin, 0.0f);
    const float selectionWidth = std::accumulate(glyphDimensions.begin() + selMin,
                                                 glyphDimensions.begin() + selMax, 0.0f);

    selectionBase->rect = {
      .x = static_cast<uint16_t>(fieldBase.rect.x + leftBorder + selectionOffset),
      .y = static_cast<uint16_t>(fieldBase.rect.y + 6),
      .width = static_cast<uint16_t>(selectionWidth),
      .height = static_cast<uint16_t>(fieldBase.rect.height - 12),
    };
    selectionBase->zOrder = 80;
  }
  else {
    selectionBase->visible = false;
  }
}

// ---- Existing methods ----

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

  if (inputField.selection == UI_NULL_ENTITY) {
    const auto selectionName = UI_strcat(entity.name().c_str(), "_selection");
    const auto selection =
      ui::ecs::createEntity(root, 0, 0, 0, 0, selectionName.c_str(), &entity);

    selection.set<ecs::QuadRendererComponent>({.color = {1.0f, 1.0f, 1.0f, 0.5f}});

    inputField.selection = selection;
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

  return 0;
}

void ui::InputField::clearSelection(ecs::InputFieldComponent &input)
{
  auto selectionBase = input.selection.get_ref<ecs::BaseComponent>();
  selectionBase->visible = false;

  input.selectionStart = input.cursorPos;
  input.selectionEnd = input.cursorPos;
}
