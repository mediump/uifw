#pragma once

#include "uifw/Core/Types.h"
#include "uifw/GFX/Renderer/RendererTypes.h"

void ui_Renderer_pickWindowPresentMode(const ui_Renderer *renderer);

void ui_Renderer_createSpriteDrawPipeline(ui_Renderer *renderer, uint32_t maxInstances);