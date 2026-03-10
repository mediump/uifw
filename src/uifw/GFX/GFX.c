#include "GFX.h"

#include "uifw/Core/Utils/Log.h"

void uifw_initGFX(void)
{
  const int sdlVersion = SDL_GetVersion();
  const char *sdlRevision = SDL_GetRevision();

  uifw_LogInfo("Initializing platform...");
  uifw_LogInfo("SDL Version: %i.%i | %s", SDL_VERSIONNUM_MAJOR(sdlVersion),
               SDL_VERSIONNUM_MINOR(sdlVersion), sdlRevision);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    uifw_LogFatal("Unable to initialize SDL: %s", SDL_GetError());
    return;
  }
}
