#include "InputHelpers.hpp"

#include "UI/Core/Application.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/GFX/Renderer/Text/TextRendererHelpers.hpp"
#include "UI/IO/Input/Input.hpp"
#include "UI/Utils/MathUtils.hpp"
#include "UI/Widgets/Text/ScrollArea.hpp"
#include "UI/Window/Window.hpp"
#include "Utils.hpp"

#include <algorithm>

constexpr float SCROLL_SPEED = 25.0f;

#define BORDER_TOP(b)    b.z
#define BORDER_BOTTOM(b) b.x
#define BORDER_LEFT(b)   b.w
#define BORDER_RIGHT(b)  b.y

using namespace ui;

SystemCursors InputHelpers::initSystemCursors()
{
  return {.defaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT),
          .pointerCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER)};
}

void InputHelpers::processEvents(const WindowData *window)
{
  const auto &inputState = window->inputState;

  if (!inputState.windowFocused && !inputState.windowResized) {
    // Ignore events when mouse is not over window and not resizing
    return;
  }

  const auto &root = window->ecsRoot;
  const auto &appStyle = window->app->appStyle;

  CursorShape cursorShape = CursorShape_Default;

  /* ---- Process events ---- */
  process_buttons(inputState, root, appStyle, &cursorShape);
  process_text_components(inputState, root);
  /* ------------------------ */

  process_cursor_update(window->app, cursorShape);
}

void InputHelpers::cleanupSystemCursors(const SystemCursors systemCursors)
{
  SDL_DestroyCursor(systemCursors.defaultCursor);
  SDL_DestroyCursor(systemCursors.pointerCursor);
}

bool InputHelpers::isMouseInRect(const Vector2i &mousePos, const Rect &rect)
{
  const auto &[p_x, p_y] = mousePos;

  return p_x >= rect.x && p_x <= rect.x + rect.width && p_y >= rect.y &&
    p_y <= rect.y + rect.height;
}

void InputHelpers::process_cursor_update(ApplicationData *app,
                                         const CursorShape &cursorShape)
{
  SDL_Cursor *targetCursor = nullptr;

  switch (cursorShape) {
  case CursorShape_Default:
    targetCursor = app->systemCursors.defaultCursor;
    break;
  case CursorShape_Pointer:
    targetCursor = app->systemCursors.pointerCursor;
    break;
  default:
    break;
  }

  const SDL_Cursor *currentCursor = SDL_GetCursor();

  if (currentCursor != targetCursor) {
    SDL_SetCursor(targetCursor);
  }
}

void InputHelpers::process_buttons(const InputState &inputState,
                                   const ecs::ECSRoot &root,
                                   const AppStyle &appStyle,
                                   CursorShape *cursorShape)
{
  if (inputState.windowResized) {
    return;
  }

  auto &world = root.world;

  const auto &mousePos = inputState.mousePosition;
  const auto &mouseDown = inputState.mouseDown;
  const auto &windowResized = inputState.windowResized;
  const auto &windowSize = inputState.windowSize;

  const auto &buttonQuery =
    world->query<ecs::ButtonComponent, ecs::QuadRendererComponent, ecs::BaseComponent,
                 ecs::HoverHandlerComponent>();

  buttonQuery.each(
    [&cursorShape, &appStyle, &mouseDown, &mousePos, &windowResized, &windowSize](
      const ecs::Entity &entity, const ecs::ButtonComponent &button,
      ecs::QuadRendererComponent &quadRenderer, const ecs::BaseComponent &base,
      const ecs::HoverHandlerComponent hoverHandler) {
      // Get clipping bounds
      Rect clippingBounds = {0, 0, windowSize.x, windowSize.y};
      const auto parent = base.transformRel.parent;

      if (parent != UI_NULL_ENTITY && parent.has<ecs::QuadRendererComponent>()) {
        const auto parentBase = parent.get<ecs::BaseComponent>();
        clippingBounds = parentBase.rect;
      }

      if (isMouseInRect(mousePos, base.rect) && isMouseInRect(mousePos, clippingBounds)) {
        const auto bgColorOpt = appStyle.buttonStyleHovered->backgroundColor;
        const auto borderColorOpt = appStyle.buttonStyleHovered->borderColor;

        ecs::Color bgColor;
        ecs::Color borderColor;

        if (bgColorOpt != std::nullopt) {
          bgColor = {
            bgColorOpt.value().r,
            bgColorOpt.value().g,
            bgColorOpt.value().b,
            bgColorOpt.value().a,
          };
        }
        else {
          bgColor = {1.0f, 1.0f, 1.0f, 1.0f};
        }

        if (borderColorOpt != std::nullopt) {
          borderColor = {
            borderColorOpt.value().r,
            borderColorOpt.value().g,
            borderColorOpt.value().b,
            borderColorOpt.value().a,
          };
        }
        else {
          borderColor = {0.0f, 0.0f, 0.0f, 0.0f};
        }

        // FIXME: Manually overriding the QuadRendererComponent color value for now,
        //  although this should be reworked later since the initial data provided by the
        //  user is lost in this process.
        if (quadRenderer.color != bgColor) {
          quadRenderer.color = bgColor;
        }

        if (quadRenderer.borderColor != borderColor) {
          quadRenderer.borderColor = borderColor;
        }

        *cursorShape = hoverHandler.cursorShape;

        // Handle click event
        if (mouseDown && button.onClick) {
          button.onClick(entity);
        }
      }
      else {
        const auto bgColorOpt = appStyle.buttonStyle->backgroundColor;
        const auto borderColorOpt = appStyle.buttonStyle->borderColor;

        ecs::Color bgColor;
        ecs::Color borderColor;

        if (bgColorOpt != std::nullopt) {
          bgColor = {
            bgColorOpt.value().r,
            bgColorOpt.value().g,
            bgColorOpt.value().b,
            bgColorOpt.value().a,
          };
        }
        else {
          bgColor = {1.0f, 1.0f, 1.0f, 1.0f};
        }

        if (borderColorOpt != std::nullopt) {
          borderColor = {
            borderColorOpt.value().r,
            borderColorOpt.value().g,
            borderColorOpt.value().b,
            borderColorOpt.value().a,
          };
        }
        else {
          borderColor = {0.0f, 0.0f, 0.0f, 0.0f};
        }

        // FIXME (See above)
        if (quadRenderer.color != bgColor) {
          quadRenderer.color = bgColor;
        }

        if (quadRenderer.borderColor != borderColor) {
          quadRenderer.borderColor = borderColor;
        }
      }
    });
}

void InputHelpers::process_text_components(const InputState &inputState,
                                           const ecs::ECSRoot &root)
{
  const auto &world = root.world;

  const auto &mousePos = inputState.mousePosition;
  const auto &mouseDown = inputState.mouseDown;
  const auto &mouseUp = inputState.mouseUp;
  const auto &scrollDelta = inputState.scrollDelta;

  const auto &textQuery = world->query<TextComponent, ecs::BaseComponent>();

  textQuery.each([&mousePos, &mouseDown, &mouseUp, &scrollDelta, &root](
                   const ecs::Entity &entity, TextComponent &textComponent,
                   const ecs::BaseComponent &base) {
    if (!textComponent.isScrollable) {
      return;
    }

    Vector4i offsets = {0, 0, 0, 0};

    if (entity.has<ecs::QuadRendererComponent>()) {
      const auto &quadRenderer = entity.get<ecs::QuadRendererComponent>();

      offsets = {
        .x = static_cast<uint16_t>(quadRenderer.borderWidths.x),
        .y = static_cast<uint16_t>(quadRenderer.borderWidths.y),
        .z = static_cast<uint16_t>(quadRenderer.borderWidths.z),
        .w = static_cast<uint16_t>(quadRenderer.borderWidths.w)
      };
    }

    if (textComponent.scrollbar == UI_NULL_ENTITY) {
      ScrollArea::addScrollbarElement(&root, entity, textComponent, base, offsets);
    }
    else {
      ScrollArea::layoutScrollbar(textComponent, base, offsets);
    }

    const float textHeight = TextUtils::computeTotalTextHeight(textComponent, base);

    const float scrollbarHeight =
      ScrollArea::updateScrollbarSize(textComponent, base, textHeight, offsets);

    // If text fits in the viewport, no scrolling is needed
    if (textHeight <= base.rect.height) {
      textComponent.scrollPosition = 0.0f;
      return;
    }

    ScrollArea::updateScrollbarPosition(textComponent, base, textHeight, offsets);
    const bool isDragging =
      ScrollArea::updateScrollbarInput(textComponent, base, mousePos, mouseDown, mouseUp);

    const float maxScrollPos = 0.0f;
    const float minScrollPos = base.rect.height - textHeight;

    if (isDragging) {
      const float scrollbarHalf = scrollbarHeight * 0.5f;
      const float mousePosRatio = 1.0f -
        std::clamp(static_cast<float>(mousePos.y - base.rect.y - scrollbarHalf) /
                     (base.rect.height - scrollbarHeight),
                   0.0f, 1.0f);

      float nextScrollPosition =
        MathUtils::remapRange(mousePosRatio, 0.0f, 1.0f, minScrollPos, maxScrollPos);
      nextScrollPosition = std::clamp(nextScrollPosition, minScrollPos, maxScrollPos);

      textComponent.scrollPosition = nextScrollPosition;
    }
    else {
      if (isMouseInRect(mousePos, base.rect)) {
        if (std::abs(scrollDelta.x) < 0.001f && std::abs(scrollDelta.y) < 0.001f) {
          // Exit early if no scroll input
          return;
        }

        float nextScrollPosition =
          textComponent.scrollPosition + scrollDelta.y * SCROLL_SPEED;

        nextScrollPosition = std::min(maxScrollPos, nextScrollPosition);
        nextScrollPosition = std::max(minScrollPos, nextScrollPosition);

        textComponent.scrollPosition = nextScrollPosition;
      }
    }
  });
}

void ui::InputHelpers::process_scrollbars(const InputState &inputState,
                                          const ecs::ECSRoot &root,
                                          const AppStyle &appStyle)
{}
