#pragma once

#include "UI/GFX/Renderer/RendererTypes.hpp"

namespace ui::ecs {

struct HoverHandlerComponent
{
  bool overridesColor = false;
  Color4f colorOverride = {1.0f, 1.0f, 1.0f, 1.0f};
};

}