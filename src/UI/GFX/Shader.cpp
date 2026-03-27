#include "Shader.hpp"

#include "UI/Window/Window.hpp"

using namespace ui;

SDL_GPUShader *ui::createShader(SDL_GPUDevice *gpuDevice,
                                const char *bytecodePath,
                                const ShaderStage stage,
                                const uint32_t samplerCount,
                                const uint32_t uniformBufferCount,
                                const uint32_t storageBufferCount,
                                const uint32_t storageTextureCount)
{
  SDL_GPUShaderStage sdlStage = SDL_GPU_SHADERSTAGE_VERTEX;
  constexpr auto ENTRY_POINT = "main";

  switch (stage) {
    case ShaderStage_Vertex:
      sdlStage = SDL_GPU_SHADERSTAGE_VERTEX;
      break;
    case ShaderStage_Fragment:
      sdlStage = SDL_GPU_SHADERSTAGE_FRAGMENT;
      break;
  }

  size_t codeSize;
  void *code = SDL_LoadFile(bytecodePath, &codeSize);

  // TODO: support formats besides SPIRV
  SDL_GPUShaderCreateInfo createInfo = {};
  createInfo.code = static_cast<uint8_t *>(code);
  createInfo.code_size = codeSize;
  createInfo.entrypoint = ENTRY_POINT;
  createInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  createInfo.stage = sdlStage;
  createInfo.num_samplers = samplerCount;
  createInfo.num_uniform_buffers = uniformBufferCount;
  createInfo.num_storage_textures = storageTextureCount;
  createInfo.num_storage_buffers = storageBufferCount;

  const auto shader = SDL_CreateGPUShader(gpuDevice, &createInfo);
  SDL_free(code);

  return shader;
}

void ui::destroyShader(SDL_GPUShader *shader, SDL_GPUDevice *gpuDevice)
{
  SDL_ReleaseGPUShader(gpuDevice, shader);
}