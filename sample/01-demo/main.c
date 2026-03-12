#include <stdio.h>

#include "uifw/Core/Utils/Log.h"
#include "uifw/uifw.h"

int main()
{
  ui_Application app = ui_initApplication();

  const ui_WindowParams winParams = {
    .title = "uifw window",
    .width = 1280,
    .height = 720,
  };
  ui_Window *window = ui_createWindow(winParams, &app);

  ui_destroyWindow(window);
}