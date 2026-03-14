#pragma once

#include "uifw/Core/Coordinates.h"

#include <SDL3/SDL.h>

/**
 * Hardware cursor types
 */
typedef enum
{
  ui_CursorShape_Default = 0,
  ui_CursorShape_Pointer = 1,
} ui_CursorShape;

/**
 * Structure of cursors for the SDL instance. Available types:
 *   - Default (arrow) cursor
 *   - Pointer cursor
 */
typedef struct
{
  SDL_Cursor *defaultCursor;
  SDL_Cursor *pointerCursor;
} ui_HardwareCursors;

/**
 * Parameters for window creation
 */
typedef struct
{
  uint16_t width;
  uint16_t height;
  const char *title;
} ui_WindowParams;

/**
 * Input state for the current window
 */
typedef struct
{
  bool shouldQuit;
  bool windowResized;
  bool mouseMoved;
  bool mouseDown;
  ui_Vector2i windowSize;
  ui_Vector2i mousePosition;
} ui_InputState;

/**
 * Native OS window
 */
typedef struct
{
  size_t id;
  SDL_Window *sdlWindow;
  ui_InputState inputState;
} ui_Window;

/**
 * Struct containing data for the overall application
 */
typedef struct
{
  ui_HardwareCursors cursors;
  size_t numWindows;
  ui_Window *windowList;
} ui_Application;


