#pragma once

#include "uifw/Core/Types.h"

#include <SDL3/SDL_gpu.h>

typedef enum
{
  ShaderStage_Vertex,
  ShaderStage_Fragment,
} ui_ShaderStage;

typedef SDL_GPUShader ui_Shader;

ui_Shader *ui_shaderCreate(const char *bytecodePath,
                           ui_ShaderStage stage,
                           uint32_t samplerCount,
                           uint32_t uniformBufferCount,
                           uint32_t storageBufferCount,
                           uint32_t storageTextureCount,
                           const ui_Renderer *renderer);

void ui_shaderDestroy(ui_Shader *shader, const ui_Renderer *renderer);