#include <iostream>

#include "UI/GFX/Shader.hpp"
#include "UI/Window/Window.hpp"

int main() {
  ui::initPlatform();

  ui::Window window = {};
  ui::initializeWindow("SDL Window", 1280, 720, &window);

  const ui::Shader shader = ui::createShader(
      "res/shaders/triangle.vs.spv",
      "res/shaders/triangle.fs.spv",
      window);

  const ui::DrawPipeline pipeline = ui::createDrawPipeline(window, shader);

  while (true) {
    if (!ui::updateWindow(&window, pipeline)) {
      break;
    }
  }

  return 0;
}
