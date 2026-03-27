#pragma once

#include "UI/IO/Input/Input.hpp"
#include "UI/ECS/Components/StyleComponents.hpp"
#include "UI/IO/Input/InputHelpers.hpp"
#include "UI/Window/Window.hpp"

#include <SDL3/SDL_gpu.h>
#include <vector>

namespace ui {

struct ApplicationData {
  bool shouldQuit = false;
  SystemCursors systemCursors;
  AppStyle appStyle;
  SDL_GPUDevice *gpuDevice = nullptr;
  std::vector<WindowData *> windows;
};

class Application
{
public:
  [[nodiscard]] static ApplicationData init();

  static bool update(ApplicationData *app);

  static void destroy(const ApplicationData *app);
};

}