#pragma once

#include "uifw/Core/Types.h"

#include <SDL3/SDL.h>

/**
 * Initialize the SDL video subsystem (do before window initialization)
 * Returns true on success, false on failure
 */
bool ui_initGFX(ui_Application *app);