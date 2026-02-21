#pragma once

#include <SDL3/SDL.h>

namespace ui {


struct Window;

struct Shader {
  SDL_GPUShader* vertexShader = nullptr;
  SDL_GPUShader* fragmentShader = nullptr;
};

Shader createShader(const char* vertexPath,
                    const char* fragmentPath,
                    const Window& window);

void destroyShader(const Shader& shader, const Window& window);

}  // namespace ui