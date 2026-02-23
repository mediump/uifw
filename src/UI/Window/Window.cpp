#include "Window.hpp"

#include "UI/GFX/Shader.hpp"
#include "Utils.hpp"

#include <cstdio>
#include <stdexcept>

void ui::initPlatform()
{
  const int version = SDL_GetVersion();
  const char *sdlRevision = SDL_GetRevision();

  SDL_Log("Initializing platform...\n");
  SDL_Log(" > SDL Version: %i.%i %s\n", SDL_VERSIONNUM_MAJOR(version),
          SDL_VERSIONNUM_MINOR(version), sdlRevision);

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    throw std::runtime_error("Unable to initialize SDL.");
  }
}

ui::DrawPipeline ui::createDrawPipeline(const Window &window,
                                        const Shader &shader)
{
  DrawPipeline drawPipeline = {};

  // Create vertex buffer
  SDL_GPUBufferCreateInfo bufferInfo = {};
  bufferInfo.size = sizeof(g_vertices);
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

  drawPipeline.vertexBuffer =
      SDL_CreateGPUBuffer(window.gpuDevice, &bufferInfo);

  // Transfer data to vertex buffer
  SDL_GPUTransferBufferCreateInfo transferInfo = {};
  transferInfo.size = sizeof(g_vertices);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

  drawPipeline.transferBuffer =
      SDL_CreateGPUTransferBuffer(window.gpuDevice, &transferInfo);

  // Map to pointer & transfer data
  auto *data = static_cast<Vertex *>(SDL_MapGPUTransferBuffer(
      window.gpuDevice, drawPipeline.transferBuffer, false));

  SDL_memcpy(data, g_vertices, sizeof(g_vertices));

  // Unmap transfer buffer
  SDL_UnmapGPUTransferBuffer(window.gpuDevice, drawPipeline.transferBuffer);

  // Now, we need to copy the data to from the transfer buffer to the vertex
  // buffer. If we needed the data to update dynamically we would do this on
  // each frame, but this data will stay static so we can just do it once
  // when the buffer is allocated.

  // Start copy pass
  SDL_GPUCommandBuffer *commandBuffer =
      SDL_AcquireGPUCommandBuffer(window.gpuDevice);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

  // Get data location
  SDL_GPUTransferBufferLocation location = {};
  location.transfer_buffer = drawPipeline.transferBuffer;
  location.offset = 0;

  // Specify upload region
  SDL_GPUBufferRegion region = {};
  region.buffer = drawPipeline.vertexBuffer;
  region.size = sizeof(g_vertices);
  region.offset = 0;

  // Upload data
  SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  // End copy pass
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);

  // Create graphics pipeline
  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.vertex_shader = shader.vertexShader;
  pipelineInfo.fragment_shader = shader.fragmentShader;
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  // Vertex buffers
  SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
  vertexBufferDescriptions[0].slot = 0;
  vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDescriptions[0].instance_step_rate = 0;
  vertexBufferDescriptions[0].pitch = sizeof(Vertex);

  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_buffer_descriptions =
      vertexBufferDescriptions;

  // Describe vertex attributes
  SDL_GPUVertexAttribute vertexAttributes[2];

  // -> Position
  vertexAttributes[0].buffer_slot = 0;
  vertexAttributes[0].location = 0;
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  vertexAttributes[0].offset = 0;

  // -> Color
  vertexAttributes[1].buffer_slot = 0;
  vertexAttributes[1].location = 1;
  vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  vertexAttributes[1].offset = sizeof(float) * 3;

  pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

  // Create color target
  SDL_GPUColorTargetDescription colorTargetDescriptions[1];

  colorTargetDescriptions[0] = {};
  colorTargetDescriptions[0].blend_state.enable_blend = true;
  colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDescriptions[0].blend_state.src_color_blendfactor =
      SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescriptions[0].blend_state.dst_color_blendfactor =
      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDescriptions[0].blend_state.src_alpha_blendfactor =
      SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor =
      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDescriptions[0].format =
      SDL_GetGPUSwapchainTextureFormat(window.gpuDevice, window.ptr);

  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

  // Create pipeline
  drawPipeline.graphicsPipeline =
      SDL_CreateGPUGraphicsPipeline(window.gpuDevice, &pipelineInfo);

  // Cleanup shaders
  destroyShader(shader, window);

  return drawPipeline;
}

ui::Rect ui::getWindowBounds(const Window *window)
{
  int x, y, width, height;

  SDL_GetWindowPosition(window->ptr, &x, &y);
  SDL_GetWindowSize(window->ptr, &width, &height);

  return Rect {
    .x = static_cast<uint16_t>(x),
    .y = static_cast<uint16_t>(y),
    .width = static_cast<uint16_t>(width),
    .height = static_cast<uint16_t>(height)
  };
}

void ui::initializeWindow(const char *title,
                          const int width,
                          const int height,
                          Window *window)
{
  // Create window
  SDL_Log("Initializing window...\n");
  window->ptr = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);

  if (window->ptr == nullptr) {
    throw std::runtime_error("Unable to create a window.");
  }
  SDL_Log(" > Window size: [%i, %i]\n", width, height);

  // Create GPU device
  SDL_Log("Creating GPU device...\n");

#ifdef _NDEBUG
  window->gpuDevice =
      SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);
#else
  window->gpuDevice =
      SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
#endif

  SDL_Log(" > Device backend: %s\n", SDL_GetGPUDeviceDriver(window->gpuDevice));
  SDL_ClaimWindowForGPUDevice(window->gpuDevice, window->ptr);

  // Create ui::Canvas
  const Rect bounds = getWindowBounds(window);
  window->canvas = createCanvas(&window->ecsRoot, bounds.x, bounds.y,
                                bounds.width, bounds.height, "Canvas");
}

inline void draw(const ui::Window *window, const ui::DrawPipeline &drawPipeline)
{
  // Acquire command buffer
  SDL_GPUCommandBuffer *commandBuffer =
      SDL_AcquireGPUCommandBuffer(window->gpuDevice);

  // Get swapchain texture
  SDL_GPUTexture *swapchainTexture;
  uint32_t width, height;

  SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window->ptr,
                                        &swapchainTexture, &width, &height);

  // End frame if swapchain texture is not available
  if (swapchainTexture == nullptr) {
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return;
  }

  // Create color target
  SDL_GPUColorTargetInfo colorTargetInfo = {};
  colorTargetInfo.clear_color = {0.3f, 0.3f, 0.3f, 1.0f};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapchainTexture;

  // Begin render pass
  SDL_GPURenderPass *renderPass =
      SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);

  // ==== Draw ====

  // Bind graphics pipeline
  SDL_BindGPUGraphicsPipeline(renderPass, drawPipeline.graphicsPipeline);

  // Bind vertex buffer
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = drawPipeline.vertexBuffer;
  bufferBindings[0].offset = 0;

  SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

  // Draw call
  SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

  // ==== End draw ====

  // End render pass
  SDL_EndGPURenderPass(renderPass);

  // Submit
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}

bool ui::updateWindow(const Window *window, const DrawPipeline &drawPipeline)
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        SDL_DestroyWindow(window->ptr);
        return false;
      default:
        break;
    }
  }

  draw(window, drawPipeline);

  return true;
}