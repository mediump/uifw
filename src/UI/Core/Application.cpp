#include "Application.hpp"

ui::ApplicationData ui::Application::init()
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

  data.systemCursors = InputHelpers::initSystemCursors();
  return data;
}

bool ui::Application::update(ApplicationData *app)
{
  if (app->windows.empty()) {
    return false;
  }

  std::vector<size_t> removeIndices;

  for (size_t i = 0; i < app->windows.size(); i++) {
    const auto window = app->windows[i];

    if (!Window::updateWindow(window)) {
      Window::destroy(window);
      removeIndices.emplace_back(i);
    }
  }

  if (!removeIndices.empty()) {
    for (size_t i = removeIndices.size() - 1; i != SIZE_MAX; i--) {
      app->windows.erase(app->windows.begin() + static_cast<long>(removeIndices[i]));
    }
  }

  return !app->windows.empty();
}

void ui::Application::destroy(const ApplicationData *app)
{
  InputHelpers::cleanupSystemCursors(app->systemCursors);
}
