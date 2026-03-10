#include "GFX.h"

#include "uifw/Core/Utils/Log.h"

#include <SDL3/SDL.h>

/**
 * Create system hardware cursors
 * @param app Ref to current ui_Application
 */
void init_hardware_cursors(ui_Application *app)
{
  app->cursors.defaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  app->cursors.pointerCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);

  ui_Assert(app->cursors.defaultCursor, "Default cursor creation failed");
  ui_Assert(app->cursors.pointerCursor, "Pointer cursor creation failed");

  ui_LogInfo("Initialized hardware cursors");
}

void ui_initGFX(ui_Application *app)
{
  const int sdlVersion = SDL_GetVersion();
  const char *sdlRevision = SDL_GetRevision();

  ui_LogInfo("Initializing platform...");
  ui_LogInfo("SDL Version: %i.%i | %s", SDL_VERSIONNUM_MAJOR(sdlVersion),
             SDL_VERSIONNUM_MINOR(sdlVersion), sdlRevision);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    ui_LogFatal("Unable to initialize SDL: %s", SDL_GetError());
    return;
  }

  init_hardware_cursors(app);
}
