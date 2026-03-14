#include "Application.h"

#include <X11/X.h>

#include "Utils/Log.h"
#include "uifw/Platform/Window.h"

ui_Application ui_initApplication(void)
{
  ui_Application app;

  app.numWindows = 0;
  app.windowList = nullptr;

  if (!ui_initGFX(&app)) {
    ui_LogFatal("Failed to initialize GFX");
  }

  return app;
}

bool ui_updateApplication(ui_Application *app)
{
  const size_t numWindows = app->numWindows;

  if (numWindows < 1 || app->windowList == nullptr) {
    return false;
  }

  // TODO: Poll events
  for (size_t i = 0; i < numWindows; ++i) {
    const ui_Window *window = &app->windowList[i];
    //ui_LogInfo("Updating window: (ID: %i)", window->id);

    if (window == nullptr) {
      continue;
    }

    ui_updateWindow(&app->windowList[i]);

    if (window->inputState.shouldQuit) {
      return false;
    }
  }

  return true;
}

void ui_destroyApplication(ui_Application *app)
{

}
