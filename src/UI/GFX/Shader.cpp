#include "Shader.hpp"

#include "UI/Window/Window.hpp"

using namespace ui;

Shader ui::createShader(const char* vertexPath,
                        const char* fragmentPath,
                        const Window& window) {
  Shader shader = {};
  
  size_t vertexCodeSize;
  void *vertexCode = SDL_LoadFile(vertexPath, &vertexCodeSize);

  SDL_GPUShaderCreateInfo vertexInfo = {};
  vertexInfo.code = static_cast<uint8_t*>(vertexCode);
  vertexInfo.code_size = vertexCodeSize;
  vertexInfo.entrypoint = "main";
  vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  vertexInfo.num_samplers = 0;
  vertexInfo.num_storage_buffers = 0;
  vertexInfo.num_uniform_buffers = 0;

  shader.vertexShader = SDL_CreateGPUShader(window.gpuDevice, &vertexInfo);
  SDL_free(vertexCode);

  size_t fragmentCodeSize;
  void *fragmentCode = SDL_LoadFile(fragmentPath, &fragmentCodeSize);

  SDL_GPUShaderCreateInfo fragmentInfo = {};
  fragmentInfo.code = static_cast<uint8_t*>(fragmentCode);
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = "main";
  fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  fragmentInfo.num_samplers = 0;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_uniform_buffers = 0;

  shader.fragmentShader = SDL_CreateGPUShader(window.gpuDevice, &fragmentInfo);
  SDL_free(fragmentCode);

  return shader;
}

void ui::destroyShader(const Shader& shader, const Window& window) {
  SDL_ReleaseGPUShader(window.gpuDevice, shader.vertexShader);
}