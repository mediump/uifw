#include "Application.hpp"

#include <SDL3/SDL_gpu.h>

using namespace ui;

ApplicationData Application::init()
{
  ApplicationData data = {};
  data.appStyle = Style::getDefaultAppStyle();

  const int version = SDL_GetVersion();
  const char *sdlRevision = SDL_GetRevision();

  SDL_Log("Initializing platform...\n");
  SDL_Log(" > SDL Version: %i.%i %s\n", SDL_VERSIONNUM_MAJOR(version),
          SDL_VERSIONNUM_MINOR(version), sdlRevision);

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    throw std::runtime_error("Unable to initialize SDL.");
  }

  SDL_Log("Creating GPU device...\n");
  data.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);

  if (data.gpuDevice == nullptr) {
    throw std::runtime_error("Unable to create GPU device.");
  }

  SDL_Log("Created GPU device (Backend: %s)\n", SDL_GetGPUDeviceDriver(data.gpuDevice));

  data.systemCursors = InputHelpers::initSystemCursors();
  return data;
}

bool Application::update(ApplicationData *app)
{
  if (app->shouldQuit) {
    destroy_all_windows(app);
    return false;
  }

  if (app->windows.empty()) {
    return false;
  }

  Input::pollEvents(app);

  std::vector<uint32_t> removeIndices;

  for (const auto &[id, window] : app->windows) {
    if (!Window::updateWindow(window)) {
      removeIndices.emplace_back(id);
    }
  }

  if (!removeIndices.empty()) {
    for (const uint32_t id : removeIndices) {
      Window::destroy(app->windows[id]);
      app->windows.erase(id);
    }
  }

  return !app->windows.empty();
}

void Application::destroy(const ApplicationData *app)
{
  InputHelpers::cleanupSystemCursors(app->systemCursors);
  
  if (app->gpuDevice != nullptr) {
    SDL_DestroyGPUDevice(app->gpuDevice);
  }
}

void Application::destroy_all_windows(ApplicationData *app) {
  for (const auto &[id, window] : app->windows) {
    Window::destroy(window);
  }

  app->windows.clear();
}