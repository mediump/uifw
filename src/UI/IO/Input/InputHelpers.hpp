#pragma once

#include "Input.hpp"

#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/ECS/Components/StyleComponents.hpp"
#include "UI/ECS/ECSRoot/ECSRoot.hpp"
#include "UI/Layout/LayoutTypes.hpp"

#include <SDL3/SDL.h>

namespace ui {

struct ApplicationData;

struct SystemCursors
{
  SDL_Cursor *defaultCursor;
  SDL_Cursor *pointerCursor;
};

class InputHelpers
{
public:
  static SystemCursors initSystemCursors();
  static void processEvents(const WindowData *window);
  static void cleanupSystemCursors(SystemCursors systemCursors);

  static bool isMouseInRect(const Vector2i &mousePos, const Rect &rect);

private:
  static void process_buttons(const InputState &inputState,
                              const ecs::ECSRoot &root,
                              const AppStyle &appStyle,
                              CursorShape *cursorShape);

  static void process_scrollbars(const InputState &inputState,
                                 const ecs::ECSRoot &root,
                                 const AppStyle &appStyle);

  static void process_text_components(const InputState &inputState,
                                      const ecs::ECSRoot &root);

  static void process_cursor_update(ApplicationData *app, const CursorShape &cursorShape);
};

} // namespace ui
