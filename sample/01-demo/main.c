#include <stdio.h>

#include "uifw/Core/Utils/Log.h"
#include "uifw/uifw.h"
#include "uifw/Core/Containers/Queue.h"

int main()
{
  ui_Application app = ui_initApplication();

  ui_Queue q;
  ui_queueInit(&q, sizeof(size_t));

  for (size_t i = 10; i > 0; i--) {
    ui_queuePush(&q, &i);
    ui_LogInfo("Pushing element: %i", i);
  }

  while (!ui_queueEmpty(&q)) {
    size_t peekOut;
    ui_queuePeek(&q, &peekOut);

    size_t popOut;
    ui_queuePop(&q, &popOut);

    ui_LogInfo("Element popped: [Peek result: %i, Pop result: %i]", peekOut, popOut);
  }

  ui_Assert(true == false, "YOU DUMB IDIOT!!!!!");

  const ui_WindowParams winParams = {
    .title = "uifw window",
    .width = 1280,
    .height = 720,
  };
  ui_Window *window = ui_createWindow(winParams, &app);

  while (ui_updateApplication(&app)) {
    // User event loop
  }

  ui_destroyWindow(window);
}