#include "RendererHelpers.h"

#include "uifw/GFX/Shader.h"

#define SPRITE_VS_PATH "res/shaders/_compiled/SPIRV/batch_render.vert.spv"
#define SPRITE_FS_PATH "res/shaders/_compiled/SPIRV/batch_render.frag.spv"

void ui_Renderer_pickWindowPresentMode(const ui_Renderer *renderer)
{
  SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

  if (SDL_WindowSupportsGPUPresentMode(renderer->gpu_device, renderer->window_ref,
                                       SDL_GPU_PRESENTMODE_IMMEDIATE)) {
    presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
  }
  else if (SDL_WindowSupportsGPUPresentMode(renderer->gpu_device, renderer->window_ref,
                                            SDL_GPU_PRESENTMODE_MAILBOX)) {
    presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
  }

  SDL_SetGPUSwapchainParameters(renderer->gpu_device, renderer->window_ref,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
}

void ui_Renderer_createSpriteDrawPipeline(ui_Renderer *renderer,
                                          const uint32_t maxInstances)
{
  SDL_GPUDevice *gpuDevice = renderer->gpu_device;
  SDL_Window *sdlWindowRef = renderer->window_ref;

  // Create shaders
  ui_Shader *spriteVertexShader =
    ui_shaderCreate(SPRITE_VS_PATH, ShaderStage_Vertex, 0, 1, 1, 0, renderer);
  ui_Shader *spriteFragmentShader =
    ui_shaderCreate(SPRITE_FS_PATH, ShaderStage_Fragment, 0, 0, 0, 0, renderer);

  // Allocate structure
  ui_Renderer_DrawPipeline *pipeline = malloc(sizeof(ui_Renderer_DrawPipeline));
  pipeline->type = DrawPipelineType_Sprite;
  pipeline->is_dirty = true;
  pipeline->size = 0;
  pipeline->pipeline = nullptr;
  pipeline->transfer_buffer = nullptr;
  pipeline->data_buffer = nullptr;
  pipeline->texture_sampler = nullptr;
  pipeline->texture = nullptr;
  pipeline->data = malloc(maxInstances * sizeof(ui_Renderer_SpriteInstance));

  // Create SDL GPU objects
  const SDL_GPUColorTargetDescription colorDesc = {
    .format = SDL_GetGPUSwapchainTextureFormat(gpuDevice, sdlWindowRef),
    .blend_state = {.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .color_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                    .enable_blend = true}};

  const SDL_GPUGraphicsPipelineTargetInfo targetInfo = {
    .color_target_descriptions = &colorDesc, .num_color_targets = 1};

  const SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
    .vertex_shader = spriteVertexShader,
    .fragment_shader = spriteFragmentShader,
    .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    .target_info = targetInfo};

  pipeline->pipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineCreateInfo);

  ui_shaderDestroy(spriteVertexShader, renderer);
  ui_shaderDestroy(spriteFragmentShader, renderer);

  // Create sprite data transfer buffer
  const SDL_GPUTransferBufferCreateInfo transferBufferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = (uint32_t)(maxInstances * sizeof(ui_Renderer_SpriteInstance))};

  pipeline->transfer_buffer = SDL_CreateGPUTransferBuffer(gpuDevice, &transferBufferInfo);

  // Create sprite data buffer
  const SDL_GPUBufferCreateInfo bufferCreateInfo = {
    .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    .size = (uint32_t)(maxInstances * sizeof(ui_Renderer_SpriteInstance))};

  pipeline->data_buffer = SDL_CreateGPUBuffer(gpuDevice, &bufferCreateInfo);

  // Set pointer in renderer
  renderer->sprite_pipeline = pipeline;
}
