#pragma once

#include "uifw/GFX/GFX.h"
#include "uifw/Core/Types.h"

[[nodiscard]] ui_Application ui_initApplication(void);

bool ui_updateApplication(ui_Application *app);

void ui_destroyApplication(ui_Application *app);


