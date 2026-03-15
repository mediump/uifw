#include "Renderer.h"

#include "uifw/Core/Utils/Log.h"
#include "uifw/Platform/Window.h"
#include "uifw/GFX/Renderer/RendererHelpers.h"

#if defined(UIFW_DEBUG)
#define UIFW_VALIDATION_ENABLED true
#else
#define UIFW_VALIDATION_ENABLED false
#endif

// Unicode Character (U+0020) Space (SP)
#define GLYPH_CHARACTER_SPACE 0x20

// Space character advance multiplier (fraction of fontSize)
constexpr float SPACE_ADVANCE_MULTIPLIER = 0.6f;

// Maximum allowed instance count per `ui_Renderer_DrawPipeline`
constexpr uint32_t MAX_INSTANCE_COUNT = 8192;

void ui_rendererCreate(ui_Window *window)
{
  // Init camera data
  ui_Renderer_CameraData cameraData;
  cameraData.is_dirty = true;

  uint16_t windowWidth, windowHeight;
  ui_getWindowSize(window, &windowWidth, &windowHeight);
  cameraData.last_window_size.x = windowWidth;
  cameraData.last_window_size.y = windowHeight;

  ui_Matrix4SetIdentity(cameraData.view_matrix);

  // Init renderer
  ui_Renderer *renderer = malloc(sizeof(ui_Renderer));
  renderer->window_ref = window->sdl_window;
  renderer->gpu_device = nullptr;
  renderer->camera_data = cameraData;
  renderer->sprite_pipeline = nullptr;
  renderer->font_pipeline = nullptr;

  // Initialize SDL GPU device
  ui_LogInfo("Creating GPU device...");
  renderer->gpu_device =
    SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, UIFW_VALIDATION_ENABLED, nullptr);

  ui_LogInfo("Created GPU device (Backend: %s)",
             SDL_GetGPUDeviceDriver(renderer->gpu_device));

  if (!SDL_ClaimWindowForGPUDevice(renderer->gpu_device,
                                   renderer->window_ref)) {
    ui_LogFatal("Failed to claim GPU device");
    return;
  }

  ui_Renderer_pickWindowPresentMode(renderer);

  // Create draw pipelines
  ui_Renderer_createSpriteDrawPipeline(renderer, MAX_INSTANCE_COUNT);

  window->renderer = renderer;
}

void ui_rendererDraw(const ui_Window *window)
{
  SDL_GPUDevice *gpuDevice = window->renderer->gpu_device;
  SDL_Window *windowRef = window->sdl_window;

  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);

  if (cmd == nullptr) {
    ui_LogFatal("Failed to acquire GPUCommandBuffer: %s", SDL_GetError());
    return;
  }

  SDL_GPUTexture *swapchainTexture;

  if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, windowRef, &swapchainTexture, nullptr,
                                             nullptr)) {
    ui_LogFatal("Failed to acquire GPUSwapchainTexture: %s", SDL_GetError());
    return;
                                             }

  if (swapchainTexture == nullptr) {
    return;
  }

  const SDL_GPUColorTargetInfo colorTargetInfo = {
    .texture = swapchainTexture,
    .clear_color = {0.1f, 0.1f, 0.1f, 1.0f},
    .load_op = SDL_GPU_LOADOP_CLEAR,
    .store_op = SDL_GPU_STOREOP_STORE,
    .cycle = false,
  };

  SDL_GPURenderPass *renderPass =
    SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, nullptr);

  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(cmd);
}
