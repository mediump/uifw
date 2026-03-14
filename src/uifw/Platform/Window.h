#pragma once

#include "uifw/Core/Types.h"

[[nodiscard]] ui_Window *ui_createWindow(ui_WindowParams params, ui_Application *app);
void ui_updateWindow(ui_Window *window);
void ui_destroyWindow(ui_Window *window);

void ui_getWindowPosition(const ui_Window *window, uint16_t *x, uint16_t *y);
void ui_getWindowSize(const ui_Window *window, uint16_t *width, uint16_t *height);