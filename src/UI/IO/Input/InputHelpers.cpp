#include "InputHelpers.hpp"

#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/Window/Window.hpp"

using namespace ui;

// FIXME: Don't use global state
static SDL_Cursor *g_defaultCursor = nullptr;
static SDL_Cursor *g_pointerCursor = nullptr;

void InputHelpers::initSystemCursors()
{
  g_defaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  g_pointerCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
}

void InputHelpers::processEvents(const Window *window)
{
  const auto &world = window->canvas.entity.world();
  const auto &appStyle = window->appStyle;

  const auto &mousePos = window->inputState.mousePosition;
  const auto &windowResized = window->inputState.windowResized;
  const auto &windowSize = window->inputState.windowSize;

  CursorShape cursorShape = CursorShape_Default;

  // PROCESS BUTTONS //////////////////////////////////
  const auto &buttonQuery =
    world.query<ecs::ButtonComponent, ecs::QuadRendererComponent, ecs::BaseComponent, ecs::HoverHandlerComponent>();

  buttonQuery.each([&cursorShape, &appStyle, &mousePos, &windowResized, &windowSize](
                     const ecs::Entity &entity,
                     const ecs::ButtonComponent &button,
                     ecs::QuadRendererComponent &quadRenderer,
                     const ecs::BaseComponent &base,
                     const ecs::HoverHandlerComponent hoverHandler) {
    // Get clipping bounds
    Rect clippingBounds = {
      0, 0, windowSize.x, windowSize.y
    };
    const auto parent = base.transformRel.parent;

    if (parent != UI_NULL_ENTITY &&
        parent.has<ecs::QuadRendererComponent>()) {
      const auto parentBase = parent.get<ecs::BaseComponent>();
      clippingBounds = parentBase.rect;
    }

    if (!windowResized && is_mouse_in_rect_component(mousePos, base.rect) &&
        is_mouse_in_rect_component(mousePos, clippingBounds)) {
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
      } else {
        bgColor = {1.0f, 1.0f, 1.0f, 1.0f};
      }

      if (borderColorOpt != std::nullopt) {
        borderColor = {
          borderColorOpt.value().r,
          borderColorOpt.value().g,
          borderColorOpt.value().b,
          borderColorOpt.value().a,
        };
      } else {
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

      cursorShape = hoverHandler.cursorShape;
    } else {
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
      } else {
        bgColor = {1.0f, 1.0f, 1.0f, 1.0f};
      }

      if (borderColorOpt != std::nullopt) {
        borderColor = {
          borderColorOpt.value().r,
          borderColorOpt.value().g,
          borderColorOpt.value().b,
          borderColorOpt.value().a,
        };
      } else {
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

  process_cursor_update(cursorShape);
}

void InputHelpers::cleanupSystemCursors()
{
  SDL_DestroyCursor(g_pointerCursor);
  SDL_DestroyCursor(g_defaultCursor);
}

bool InputHelpers::is_mouse_in_rect_component(const Vector2i &mousePos, const Rect &rect)
{
  const auto &[p_x, p_y] = mousePos;

  return p_x >= rect.x &&
         p_x <= rect.x + rect.width &&
         p_y >= rect.y &&
         p_y <= rect.y + rect.height;
}

void InputHelpers::process_cursor_update(const CursorShape &cursorShape)
{
  SDL_Cursor *targetCursor = nullptr;

  switch (cursorShape) {
  case CursorShape_Default:
    targetCursor = g_defaultCursor;
    break;
  case CursorShape_Pointer:
    targetCursor = g_pointerCursor;
    break;
  default:
    break;
  }

  const SDL_Cursor *currentCursor = SDL_GetCursor();

  if (currentCursor != targetCursor) {
    SDL_SetCursor(targetCursor);
  }
}
