#pragma once

#include "uifw/Core/Coordinates.h"
#include "uifw/GFX/Renderer/RendererTypes.h"
#include "uifw/ECS/Entity.h"
#include "uifw/ECS/World.h"

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
  bool first_frame;
  bool should_quit;
  bool window_resized;
  bool mouse_moved;
  bool mouse_down;
  ui_Vector2i window_size;
  ui_Vector2i mouse_position;
} ui_InputState;

typedef struct
{
  ui_ECS_World *world;
  ui_ECS_Entity root_canvas;
} ui_Scene;

typedef struct
{
  clock_t current_ticks;
  clock_t delta_ticks;
  clock_t fps;
} ui_Time;

/**
 * Native OS window
 */
typedef struct
{
  size_t id;
  SDL_Window *sdl_window;
  ui_Scene scene;
  ui_Renderer *renderer;
  ui_InputState input_state;
  ui_Time time_state;
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


