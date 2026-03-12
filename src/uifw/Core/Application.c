#include "Application.h"

ui_Application ui_initApplication(void)
{
  ui_Application app;

  app.numWindows = 0;
  app.windowList = nullptr;

  ui_initGFX(&app);

  return app;
}

void ui_updateApplication(ui_Application *app)
{
  if (app->numWindows < 1 || app->windowList == nullptr) {
    return;
  }


}

void ui_destroyApplication(ui_Application *app)
{

}
