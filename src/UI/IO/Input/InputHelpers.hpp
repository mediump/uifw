#pragma once

#include "Input.hpp"

#include "UI/ECS/Components/InputComponents.hpp"
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

private:
  static bool is_mouse_in_rect_component(const Vector2i &mousePos, const Rect &rect);
  static void process_cursor_update(ApplicationData *app, const CursorShape &cursorShape);
};

} // namespace ui
