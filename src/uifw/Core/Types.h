#pragma once

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
 * Native OS window
 */
typedef struct
{
  SDL_Window *sdlWindow;
  size_t id;
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


