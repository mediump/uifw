#pragma once

#include <SDL3/SDL_gpu.h>

#include "UI/GFX/Renderer/Renderer.hpp"

namespace ui {

struct WindowData;

enum ShaderStage
{
  ShaderStage_Vertex,
  ShaderStage_Fragment,
};

typedef SDL_GPUShader Shader;

Shader *createShader(SDL_GPUDevice *gpuDevice,
                     const char *bytecodePath,
                     ShaderStage stage,
                     uint32_t samplerCount,
                     uint32_t uniformBufferCount,
                     uint32_t storageBufferCount,
                     uint32_t storageTextureCount);

void destroyShader(Shader *shader, SDL_GPUDevice *gpuDevice);

}  // namespace ui