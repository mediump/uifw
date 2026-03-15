#include "Shader.h"

#define ENTRY_POINT "main"

ui_Shader *ui_shaderCreate(const char *bytecodePath,
                           const ui_ShaderStage stage,
                           const uint32_t samplerCount,
                           const uint32_t uniformBufferCount,
                           const uint32_t storageBufferCount,
                           const uint32_t storageTextureCount,
                           const ui_Renderer *renderer)
{
  SDL_GPUShaderStage sdlStage = SDL_GPU_SHADERSTAGE_VERTEX;

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
  createInfo.code = (uint8_t *)code;
  createInfo.code_size = codeSize;
  createInfo.entrypoint = ENTRY_POINT;
  createInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  createInfo.stage = sdlStage;
  createInfo.num_samplers = samplerCount;
  createInfo.num_uniform_buffers = uniformBufferCount;
  createInfo.num_storage_textures = storageTextureCount;
  createInfo.num_storage_buffers = storageBufferCount;

  const auto shader = SDL_CreateGPUShader(renderer->gpu_device, &createInfo);
  SDL_free(code);

  return shader;
}

void ui_shaderDestroy(ui_Shader *shader, const ui_Renderer *renderer)
{
  SDL_ReleaseGPUShader(renderer->gpu_device, shader);
}
