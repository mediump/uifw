#include "Renderer.hpp"

#include "Text/TextTypes.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/GFX/Renderer/RendererTypes.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/IO/Image/Image.hpp"
#include "UI/Window/Window.hpp"
#include "Utils.hpp"

#include <SDL3/SDL_gpu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/IO/Input/InputHelpers.hpp"

#ifdef UI_DEBUG_ENABLED
#define UI_VALIDATION_ENABLED true
#else
#define UI_VALIDATION_ENABLED false
#endif

// Unicode Character (U+0020) Space (SP)
#define GLYPH_CHARACTER_SPACE 0x20

// Space character advance multiplier (fraction of fontSize)
constexpr float SPACE_ADVANCE_MULTIPLIER = 0.6f;

constexpr uint32_t MAX_SPRITE_COUNT = 8192;
constexpr uint32_t MAX_GLYPH_COUNT = 8192;

using namespace ui;

void Renderer::pick_window_present_mode(const RendererData *renderer)
{
  SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

  if (SDL_WindowSupportsGPUPresentMode(renderer->internals.gpuDevice,
                                       renderer->internals.sdlWindowPtr,
                                       SDL_GPU_PRESENTMODE_IMMEDIATE)) {
    presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
  }
  else if (SDL_WindowSupportsGPUPresentMode(renderer->internals.gpuDevice,
                                            renderer->internals.sdlWindowPtr,
                                            SDL_GPU_PRESENTMODE_MAILBOX)) {
    presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
  }

  SDL_SetGPUSwapchainParameters(renderer->internals.gpuDevice,
                                renderer->internals.sdlWindowPtr,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
}

void Renderer::create_text_render_pipeline(const RendererData *renderer,
                                           const Canvas *canvas,
                                           DrawPipeline *pipeline)
{
  auto *gpuDevice = renderer->internals.gpuDevice;
  auto *window = renderer->internals.sdlWindowPtr;

  constexpr auto TEXT_VS_PATH = "res/shaders/_compiled/SPIRV/batch_render_font.vert.spv";
  constexpr auto TEXT_FS_PATH = "res/shaders/_compiled/SPIRV/batch_render_font.frag.spv";

  // Load font atlas image
  SDL_Surface *imageData = Image::loadImageFromPath(
    "res/fonts/_generated/JetBrainsMono.png", SDL_PIXELFORMAT_ARGB8888);

  if (imageData == nullptr) {
    UI_LOG_MSG("No font atlas could be loaded. Exiting...");
    exit(0);
  }

  SDL_GPUShader *textVertexShader =
    createShader(renderer, TEXT_VS_PATH, ShaderStage_Vertex, 0, 1, 1, 0);
  SDL_GPUShader *textFragmentShader =
    createShader(renderer, TEXT_FS_PATH, ShaderStage_Fragment, 1, 0, 0, 0);

  const SDL_GPUColorTargetDescription colorDesc = {
    .format = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window),
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
    .vertex_shader = textVertexShader,
    .fragment_shader = textFragmentShader,
    .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    .target_info = targetInfo};

  pipeline->textPipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineCreateInfo);

  destroyShader(textVertexShader, renderer);
  destroyShader(textFragmentShader, renderer);

  // Create font atlas texture
  const auto imageDataSize = static_cast<uint32_t>(imageData->w * imageData->h * 4);

  SDL_GPUTransferBufferCreateInfo atlasTransferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = imageDataSize,
  };

  SDL_GPUTransferBuffer *textureTransferBuffer =
    SDL_CreateGPUTransferBuffer(gpuDevice, &atlasTransferInfo);

  void *textureTransferPtr =
    SDL_MapGPUTransferBuffer(gpuDevice, textureTransferBuffer, false);
  SDL_memcpy(textureTransferPtr, imageData->pixels, imageDataSize);
  SDL_UnmapGPUTransferBuffer(gpuDevice, textureTransferBuffer);

  const SDL_GPUTextureCreateInfo atlasTexCreateInfo = {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
    .width = static_cast<uint32_t>(imageData->w),
    .height = static_cast<uint32_t>(imageData->h),
    .layer_count_or_depth = 1,
    .num_levels = 1,
  };

  pipeline->fontAtlasTexture = SDL_CreateGPUTexture(gpuDevice, &atlasTexCreateInfo);

  constexpr SDL_GPUSamplerCreateInfo atlasSamplerCreateInfo = {
    .min_filter = SDL_GPU_FILTER_LINEAR,
    .mag_filter = SDL_GPU_FILTER_LINEAR,
    .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
    .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
  };

  pipeline->fontAtlasSampler = SDL_CreateGPUSampler(gpuDevice, &atlasSamplerCreateInfo);

  // Create text data transfer buffer
  constexpr SDL_GPUTransferBufferCreateInfo textTransferBufferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = MAX_GLYPH_COUNT * sizeof(FontGlyphInstance),
  };

  pipeline->textTransferBuffer =
    SDL_CreateGPUTransferBuffer(gpuDevice, &textTransferBufferInfo);

  constexpr SDL_GPUBufferCreateInfo textBufferCreateInfo = {
    .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    .size = MAX_GLYPH_COUNT * sizeof(FontGlyphInstance),
  };

  pipeline->textBuffer = SDL_CreateGPUBuffer(gpuDevice, &textBufferCreateInfo);

  // Upload font atlas texture
  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

  const SDL_GPUTextureTransferInfo transferInfo = {
    .transfer_buffer = textureTransferBuffer,
    .offset = 0,
  };

  const SDL_GPUTextureRegion transferRegion = {.texture = pipeline->fontAtlasTexture,
                                               .w = static_cast<uint32_t>(imageData->w),
                                               .h = static_cast<uint32_t>(imageData->h),
                                               .d = 1};

  SDL_UploadToGPUTexture(copyPass, &transferInfo, &transferRegion, false);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(cmd);

  SDL_DestroySurface(imageData);
  SDL_ReleaseGPUTransferBuffer(gpuDevice, textureTransferBuffer);
}

size_t Renderer::record_sprite_draw_list(const Window *window,
                                         std::vector<SpriteInstance> &outInstances)
{
  const Canvas &canvas = window->canvas;
  const InputState &inputState = window->inputState;

  const auto world = canvas.entity.world();
  const auto quadQuery = world.query<ecs::BaseComponent, ecs::QuadRenderer>();

  // Reserve space to avoid reallocations
  const size_t estimatedCount = quadQuery.count();
  if (outInstances.capacity() < estimatedCount) {
    outInstances.reserve(estimatedCount);
  }
  outInstances.clear();

  size_t counter = 0;

  quadQuery.each([&inputState, &outInstances, &counter](
                   ecs::Entity e, const ecs::BaseComponent &baseComponent,
                   const ecs::QuadRenderer &quadRenderer) {
    if (counter >= MAX_SPRITE_COUNT) {
      return;
    }

    Color4f color = {
      quadRenderer.color.r,
      quadRenderer.color.g,
      quadRenderer.color.b,
      quadRenderer.color.a,
    };

    if (e.has<ecs::HoverHandlerComponent>()) {
      const auto &hoverHandler = e.get<ecs::HoverHandlerComponent>();

      if (hoverHandler.overridesColor &&
          InputHelpers::isMouseInRectComponent(inputState, baseComponent.rect)) {
        color = hoverHandler.colorOverride;
      }
    }

    outInstances.emplace_back(SpriteInstance{
      .position = {static_cast<float>(baseComponent.rect.x),
                   static_cast<float>(baseComponent.rect.y),
                   static_cast<float>(baseComponent.zOrder)},
      .rotation = 0.0f,
      .size =
        {
          static_cast<float>(baseComponent.rect.width),
          static_cast<float>(baseComponent.rect.height),
        },
      .color = color,
    });
    counter++;
  });

  return counter;
}


size_t Renderer::record_glyph_draw_list(const Canvas *canvas,
                                        std::vector<FontGlyphInstance> &outInstances)
{
  const auto world = canvas->entity.world();
  const auto textQuery = world.query<ecs::BaseComponent, TextComponent>();

  // Reserve space to avoid reallocations
  outInstances.clear();
  outInstances.reserve(MAX_GLYPH_COUNT);

  size_t counter = 0;

  textQuery.each([&outInstances, &counter](ecs::Entity e,
                                           const ecs::BaseComponent &baseComponent,
                                           const TextComponent &textComponent) {
    const auto textView = std::string_view(textComponent.text);
    const auto fontData = textComponent.font;
    const auto fontSize = static_cast<float>(textComponent.pixelSize);
    const auto textureSize =
      Vector2f{static_cast<float>(fontData->atlas.atlasDimensions.x),
               static_cast<float>(fontData->atlas.atlasDimensions.y)};

    const Color4f color = textComponent.color;

    float currentAdvance = 0.0f;
    float currentBaselineY =
      static_cast<float>(baseComponent.rect.y) + (fontData->metrics.ascender * fontSize);

    const char *strPtr = textView.data();
    size_t strLen = textView.size();

    while (strLen > 0 && counter < MAX_GLYPH_COUNT) {
      const uint32_t unicodeValue = SDL_StepUTF8(&strPtr, &strLen);

      // Handle space character separately (no glyph bounds, only advance)
      if (unicodeValue == GLYPH_CHARACTER_SPACE) {
        currentAdvance += SPACE_ADVANCE_MULTIPLIER * fontSize;
        continue;
      }

      // Check if glyph exists in the font atlas
      auto glyphIt = fontData->glyphs.find(unicodeValue);
      if (glyphIt == fontData->glyphs.end()) {
        continue;
      }
      const auto &glyphData = glyphIt->second;

      const float pl = glyphData.planeBounds.left * fontSize;
      const float pt = glyphData.planeBounds.top * fontSize;
      const float pr = glyphData.planeBounds.right * fontSize;
      const float pb = glyphData.planeBounds.bottom * fontSize;

      const float quadWidth = pr - pl;
      const float quadHeight = pt - pb;

      const float atlasHeight = textureSize.y;

      const Vector4f textureCoords = {
        glyphData.atlasBounds.left / textureSize.x,
        (atlasHeight - glyphData.atlasBounds.top) / atlasHeight,
        glyphData.atlasBounds.right / textureSize.x,
        (atlasHeight - glyphData.atlasBounds.bottom) / atlasHeight,
      };

      outInstances.emplace_back(FontGlyphInstance{
        .position =
          {
            .x = static_cast<float>(baseComponent.rect.x) + currentAdvance + pl,
            .y = currentBaselineY - pt,
            .z = static_cast<float>(baseComponent.zOrder),
          },
        .size =
          {
            .x = quadWidth,
            .y = quadHeight,
          },
        .textureCoords = textureCoords,
        .color = color,
      });

      currentAdvance += glyphData.advance * fontSize;
      counter++;
    }
  });

  return counter;
}

const glm::mat4 &Renderer::get_camera_matrix(DrawDataStorage &storage,
                                             uint16_t windowWidth,
                                             uint16_t windowHeight)
{
  // Return cached matrix if window dimensions haven't changed
  if (storage.cameraMatrixValid && storage.lastWindowWidth == windowWidth &&
      storage.lastWindowHeight == windowHeight) {
    return storage.cachedCameraMatrix;
  }

  // Calculate new camera matrix
  storage.cachedCameraMatrix =
    glm::orthoZO(0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight),
                 0.0f, -1000.0f, 1000.0f);

  storage.lastWindowWidth = windowWidth;
  storage.lastWindowHeight = windowHeight;
  storage.cameraMatrixValid = true;

  return storage.cachedCameraMatrix;
}

DrawPipeline Renderer::create_draw_pipeline(const RendererData *renderer,
                                            const Canvas *canvas)
{
  DrawPipeline pipeline = {};

  auto *gpuDevice = renderer->internals.gpuDevice;
  auto *window = renderer->internals.sdlWindowPtr;

  // SPRITE RENDER PIPELINE
  constexpr auto SPRITE_VS_PATH = "res/shaders/_compiled/SPIRV/batch_render.vert.spv";
  constexpr auto SPRITE_FS_PATH = "res/shaders/_compiled/SPIRV/batch_render.frag.spv";

  SDL_GPUShader *spriteVertexShader =
    createShader(renderer, SPRITE_VS_PATH, ShaderStage_Vertex, 0, 1, 1, 0);
  SDL_GPUShader *spriteFragmentShader =
    createShader(renderer, SPRITE_FS_PATH, ShaderStage_Fragment, 0, 0, 0, 0);

  const SDL_GPUColorTargetDescription colorDesc = {
    .format = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window),
    .blend_state = {.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .color_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                    .enable_blend = true}};

  const SDL_GPUGraphicsPipelineTargetInfo targetInfo = {
    .color_target_descriptions = &colorDesc, .num_color_targets = 1};

  SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
    .vertex_shader = spriteVertexShader,
    .fragment_shader = spriteFragmentShader,
    .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
    .target_info = targetInfo};

  pipeline.spriteDataPipeline =
    SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineCreateInfo);

  destroyShader(spriteVertexShader, renderer);
  destroyShader(spriteFragmentShader, renderer);

  // Create sprite data transfer buffer
  constexpr SDL_GPUTransferBufferCreateInfo transferBufferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = static_cast<uint32_t>(MAX_SPRITE_COUNT * sizeof(SpriteInstance))};

  pipeline.spriteDataTransferBuffer =
    SDL_CreateGPUTransferBuffer(gpuDevice, &transferBufferInfo);

  // Create sprite data buffer
  constexpr SDL_GPUBufferCreateInfo bufferCreateInfo = {
    .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    .size = static_cast<uint32_t>(MAX_SPRITE_COUNT * sizeof(SpriteInstance))};

  pipeline.spriteDataBuffer = SDL_CreateGPUBuffer(gpuDevice, &bufferCreateInfo);

  // Create text render pipeline
  create_text_render_pipeline(renderer, canvas, &pipeline);

  return pipeline;
}

RendererData Renderer::createRenderer(const Window *window, const Canvas *canvas)
{
  RendererData renderer;

  renderer.internals.sdlWindowPtr = window->ptr;

  SDL_Log("Creating GPU device...\n");

  renderer.internals.gpuDevice =
    SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, UI_VALIDATION_ENABLED, nullptr);

  UI_LOG_MSG("Created GPU device (Backend: %s)",
             SDL_GetGPUDeviceDriver(renderer.internals.gpuDevice));

  if (!SDL_ClaimWindowForGPUDevice(renderer.internals.gpuDevice,
                                   renderer.internals.sdlWindowPtr)) {
    UI_LOG_MSG("Failed to claim GPU device");
    return renderer;
  }

  pick_window_present_mode(&renderer);

  renderer.drawPipeline = create_draw_pipeline(&renderer, canvas);

  // Pre-allocate storage buffers
  renderer.storage.spriteInstances.reserve(MAX_SPRITE_COUNT);
  renderer.storage.glyphInstances.reserve(MAX_GLYPH_COUNT);

  return renderer;
}

void Renderer::draw(Window *window)
{
  auto *gpuDevice = window->renderer.internals.gpuDevice;
  auto *windowPtr = window->renderer.internals.sdlWindowPtr;
  auto &drawPipeline = window->renderer.drawPipeline;
  auto &storage = window->renderer.storage;

  // Get window bounds for camera matrix
  const Rect windowBounds = getWindowBounds(window);

  // Get cached camera matrix (only recalculated on resize)
  const glm::mat4 &cameraMatrix =
    get_camera_matrix(storage, windowBounds.width, windowBounds.height);

  // Get command buffer
  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);

  if (cmd == nullptr) {
    UI_LOG_MSG("Failed to acquire GPUCommandBuffer: %s", SDL_GetError());
    return;
  }

  SDL_GPUTexture *swapchainTexture;

  if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, windowPtr, &swapchainTexture, nullptr,
                                             nullptr)) {
    UI_LOG_MSG("Failed to acquire GPUSwapchainTexture: %s", SDL_GetError());
    return;
  }

  if (swapchainTexture == nullptr) {
    return;
  }

  // Record sprite draw list into pre-allocated storage
  const size_t spriteCountToRender =
    record_sprite_draw_list(window, storage.spriteInstances);

  // Record glyph draw list into pre-allocated storage
  const size_t glyphCountToRender =
    record_glyph_draw_list(&window->canvas, storage.glyphInstances);

  // Map both transfer buffers
  auto *spriteDataPtr = static_cast<SpriteInstance *>(
    SDL_MapGPUTransferBuffer(gpuDevice, drawPipeline.spriteDataTransferBuffer, true));
  auto *textDataPtr = static_cast<FontGlyphInstance *>(
    SDL_MapGPUTransferBuffer(gpuDevice, drawPipeline.textTransferBuffer, true));

  // Copy data to transfer buffers
  if (spriteCountToRender > 0) {
    SDL_memcpy(spriteDataPtr, storage.spriteInstances.data(),
               spriteCountToRender * sizeof(SpriteInstance));
  }

  if (glyphCountToRender > 0) {
    SDL_memcpy(textDataPtr, storage.glyphInstances.data(),
               glyphCountToRender * sizeof(FontGlyphInstance));
  }

  SDL_UnmapGPUTransferBuffer(gpuDevice, drawPipeline.spriteDataTransferBuffer);
  SDL_UnmapGPUTransferBuffer(gpuDevice, drawPipeline.textTransferBuffer);

  // Single copy pass for both sprite and text data
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

  // Upload sprite instance data
  const SDL_GPUTransferBufferLocation spriteTransferBufferLocation = {
    .transfer_buffer = drawPipeline.spriteDataTransferBuffer, .offset = 0};

  const SDL_GPUBufferRegion spriteBufferRegion = {
    .buffer = drawPipeline.spriteDataBuffer,
    .offset = 0,
    .size = static_cast<uint32_t>(spriteCountToRender * sizeof(SpriteInstance))};

  SDL_UploadToGPUBuffer(copyPass, &spriteTransferBufferLocation, &spriteBufferRegion,
                        false); // false = not last transfer

  // Upload text instance data
  const SDL_GPUTransferBufferLocation textTransferBufferLocation = {
    .transfer_buffer = drawPipeline.textTransferBuffer, .offset = 0};

  const SDL_GPUBufferRegion textBufferRegion = {
    .buffer = drawPipeline.textBuffer,
    .offset = 0,
    .size = static_cast<uint32_t>(glyphCountToRender * sizeof(FontGlyphInstance))};

  SDL_UploadToGPUBuffer(copyPass, &textTransferBufferLocation, &textBufferRegion,
                        true); // true = last transfer

  SDL_EndGPUCopyPass(copyPass);

  // Render everything
  const SDL_GPUColorTargetInfo colorTargetInfo = {
    .texture = swapchainTexture,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
    .load_op = SDL_GPU_LOADOP_CLEAR,
    .store_op = SDL_GPU_STOREOP_STORE,
    .cycle = false,
  };

  SDL_GPURenderPass *renderPass =
    SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, nullptr);

  // Push camera matrix uniform (once for both passes)
  SDL_PushGPUVertexUniformData(cmd, 0, &cameraMatrix, sizeof(glm::mat4));

  // SPRITE RENDERING
  SDL_BindGPUGraphicsPipeline(renderPass, drawPipeline.spriteDataPipeline);
  SDL_BindGPUVertexStorageBuffers(renderPass, 0, &drawPipeline.spriteDataBuffer, 1);
  SDL_DrawGPUPrimitives(renderPass, spriteCountToRender * 6, 1, 0, 0);

  // TEXT RENDERING
  SDL_BindGPUGraphicsPipeline(renderPass, drawPipeline.textPipeline);
  SDL_BindGPUVertexStorageBuffers(renderPass, 0, &drawPipeline.textBuffer, 1);

  const SDL_GPUTextureSamplerBinding fontAtlasTexBinding = {
    .texture = drawPipeline.fontAtlasTexture,
    .sampler = drawPipeline.fontAtlasSampler,
  };
  SDL_BindGPUFragmentSamplers(renderPass, 0, &fontAtlasTexBinding, 1);

  SDL_DrawGPUPrimitives(renderPass, glyphCountToRender * 6, 1, 0, 0);

  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(cmd);
}
