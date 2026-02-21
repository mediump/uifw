#pragma once

#include <SDL3/SDL.h>

namespace ui {

struct Shader;

struct Window {
  SDL_Window* ptr;
  SDL_GPUDevice* gpuDevice;
};

struct DrawPipeline {
  SDL_GPUBuffer* vertexBuffer;
  SDL_GPUTransferBuffer* transferBuffer;
  SDL_GPUGraphicsPipeline* graphicsPipeline;
};

struct Vertex {
  float x, y, z;
  float r, g, b, a;
};

static Vertex g_vertices[]{{0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
                           {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},
                           {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}};

void initPlatform();

[[nodiscard]] DrawPipeline createDrawPipeline(const Window& window,
                                              const Shader& shader);

void initializeWindow(const char* title, int width, int height, Window* window);

bool updateWindow(const Window* window, const DrawPipeline& drawPipeline);

}  // namespace ui