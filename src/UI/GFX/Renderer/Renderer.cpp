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
#include <vector>

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
  // SDL_GPUPresent mode is used to determine how the swapchain will present its textures
  // to the OS.
  //
  // Ref: https://wiki.libsdl.org/SDL3/SDL_GPUPresentMode
  //
  SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

  if (SDL_WindowSupportsGPUPresentMode(renderer->internals.gpuDevice,
                                       renderer->internals.sdlWindowPtr,
                                       SDL_GPU_PRESENTMODE_IMMEDIATE)) {
    // Immediate present mode does what you think it does. Provides the lowest latency
    // option, but tends to cause screen tearing.
    //
    presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
  }
  else if (SDL_WindowSupportsGPUPresentMode(renderer->internals.gpuDevice,
                                            renderer->internals.sdlWindowPtr,
                                            SDL_GPU_PRESENTMODE_MAILBOX)) {
    // Mailbox awaits vblank for presentation, eliminating screen tearing. Thus, it is
    // the preferred default.
    //
    presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
  }

  SDL_SetGPUSwapchainParameters(renderer->internals.gpuDevice,
                                renderer->internals.sdlWindowPtr,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
}

size_t Renderer::get_text_render_instance_count_from_query(
  const flecs::query<ecs::BaseComponent, TextComponent> &query)
{
  size_t counter = 0;

  query.each([&counter](const ecs::Entity e, const ecs::BaseComponent &baseComponent,
                        const TextComponent &textComponent) {
    counter += strlen(textComponent.text);
  });

  return counter;
}

void Renderer::create_text_render_pipeline(const RendererData *renderer,
                                           const Canvas *canvas,
                                           DrawPipeline *pipeline)
{
  auto *gpuDevice = renderer->internals.gpuDevice;
  auto *window = renderer->internals.sdlWindowPtr;

  constexpr auto TEXT_VS_PATH = "res/shaders/_compiled/SPIRV/batch_render_font.vert.spv";
  constexpr auto TEXT_FS_PATH = "res/shaders/_compiled/SPIRV/batch_render_font.frag.spv";

  // Load image data for font
  //
  // TODO: Find a way to load fonts into a global registry so we are not required
  //  to manually provide a path here.
  //
  SDL_Surface *imageData = Image::loadImageFromPath(
    "res/fonts/_generated/JetBrainsMono.png",
    SDL_PIXELFORMAT_ARGB8888);

  if (imageData == nullptr) {
    UI_LOG_MSG("No font atlas could be loaded. Exiting...");
    // Cleanup shaders before exiting
    // Note: Shaders haven't been created yet at this point
    exit(0); // FIXME: Fix dumb idiot exit
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

  // Cleanup shaders
  destroyShader(textVertexShader, renderer);
  destroyShader(textFragmentShader, renderer);

  const auto imageDataSize = static_cast<uint32_t>(imageData->w * imageData->h * 4);

  SDL_GPUTransferBufferCreateInfo atlasTransferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = imageDataSize,
  };

  SDL_GPUTransferBuffer *textureTransferBuffer =
    SDL_CreateGPUTransferBuffer(gpuDevice, &atlasTransferInfo);

  void *textureTransferPtr = SDL_MapGPUTransferBuffer(
    gpuDevice, textureTransferBuffer, false);
  SDL_memcpy(textureTransferPtr, imageData->pixels, imageDataSize);
  SDL_UnmapGPUTransferBuffer(gpuDevice, textureTransferBuffer);

  // Create GPU texture and sampler
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

  // Build draw list
  const auto textDrawList = record_glyph_draw_list(canvas);
  const auto textDrawListSize =
    static_cast<uint32_t>(textDrawList.size() * sizeof(FontGlyphInstance));

  // Create data/transfer buffers
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

  // Transfer upfront data
  SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpuDevice);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmd);

  const SDL_GPUTextureTransferInfo transferInfo = {
    .transfer_buffer = textureTransferBuffer,
    .offset = 0,
  };

  const SDL_GPUTextureRegion transferRegion = {
    .texture = pipeline->fontAtlasTexture,
    .w = static_cast<uint32_t>(imageData->w),
    .h = static_cast<uint32_t>(imageData->h),
    .d = 1
  };

  SDL_UploadToGPUTexture(copyPass, &transferInfo, &transferRegion, false);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(cmd);

  SDL_DestroySurface(imageData);
  SDL_ReleaseGPUTransferBuffer(gpuDevice, textureTransferBuffer);
}

std::vector<SpriteInstance> Renderer::record_sprite_draw_list(const Canvas *canvas)
{
  const auto world = canvas->entity.world();

  const auto quadQuery = world.query<ecs::BaseComponent, ecs::QuadRenderer>();
  const size_t totalInstanceCount = quadQuery.count();

  std::vector<SpriteInstance> instanceList(totalInstanceCount);
  size_t counter = 0;

  if (instanceList.empty()) {
    return instanceList;
  }

  // Querying quads
  quadQuery.each([&instanceList, &counter](ecs::Entity e,
                                           const ecs::BaseComponent &baseComponent,
                                           const ecs::QuadRenderer &quadRenderer) {
    instanceList[counter] = {
      .position = {static_cast<float>(baseComponent.rect.x),
                   static_cast<float>(baseComponent.rect.y),
                   static_cast<float>(baseComponent.zOrder)},
      .rotation = 0.0f,
      .size =
        {
          static_cast<float>(baseComponent.rect.width),
          static_cast<float>(baseComponent.rect.height),
        },
      .color =
        {
          quadRenderer.color.r,
          quadRenderer.color.g,
          quadRenderer.color.b,
          quadRenderer.color.a,
        },
    };

    counter++;
  });

  return instanceList;
}

std::vector<FontGlyphInstance> Renderer::record_glyph_draw_list(const Canvas *canvas)
{
  const auto world = canvas->entity.world();

  const auto textQuery = world.query<ecs::BaseComponent, TextComponent>();
  const size_t glyphCount = get_text_render_instance_count_from_query(textQuery);

  std::vector<FontGlyphInstance> instanceList(glyphCount);
  size_t counter = 0;

  if (instanceList.empty()) {
    return instanceList;
  }

  textQuery.each([&instanceList, &counter](ecs::Entity e,
                                           const ecs::BaseComponent &baseComponent,
                                           const TextComponent &textComponent) {
    const auto textView = std::string_view(textComponent.text);
    const auto fontData = textComponent.font;
    const auto fontSize = static_cast<float>(textComponent.pixelSize);
    const auto textureSize = Vector2f {
      static_cast<float>(fontData->atlas.atlasDimensions.x),
      static_cast<float>(fontData->atlas.atlasDimensions.y)
    };

    constexpr Color4f color = {1.0f, 1.0f, 1.0f, 1.0f};
    const auto lineHeight = fontData->metrics.lineHeight;


    float currentAdvance = 0.0f;
    float currentBaselineY = static_cast<float>(baseComponent.rect.y) +
      (fontData->metrics.ascender * fontSize);

    const char *strPtr = textView.data();
    size_t strLen = textView.size();

    while (strLen > 0) {
      const uint32_t unicodeValue = SDL_StepUTF8(&strPtr, &strLen);

      // Handle space character separately
      if (unicodeValue == GLYPH_CHARACTER_SPACE) {
        currentAdvance += SPACE_ADVANCE_MULTIPLIER * fontSize;
        continue;
      }

      // Check if glyph exists in the font atlas
      auto glyphIt = fontData->glyphs.find(unicodeValue);
      if (glyphIt == fontData->glyphs.end()) {
        // Skip missing glyphs
        // TODO: show missing character glyph
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


      instanceList[counter] = {
        .position = {
            .x = static_cast<float>(baseComponent.rect.x) + currentAdvance + pl,
            .y = currentBaselineY - pt,
            .z = static_cast<float>(baseComponent.zOrder),
        },
        .size = {
            .x = quadWidth,
            .y = quadHeight,
        },
        .textureCoords = textureCoords,
        .color = color,
    };

      currentAdvance += glyphData.advance * fontSize;
      counter++;
    }
  });

  return instanceList;
}

DrawPipeline Renderer::create_draw_pipeline(const RendererData *renderer,
                                            const Canvas *canvas)
{
  DrawPipeline pipeline = {};

  auto *gpuDevice = renderer->internals.gpuDevice;
  auto *window = renderer->internals.sdlWindowPtr;

  // SPRITE RENDER PIPELINE ////////////////////////////
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

  // Cleanup shaders
  destroyShader(spriteVertexShader, renderer);
  destroyShader(spriteFragmentShader, renderer);

  // Build render pipeline
  const auto spriteDrawList = record_sprite_draw_list(canvas);

  // Create data transfer buffer
  constexpr SDL_GPUTransferBufferCreateInfo transferBufferInfo = {
    .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    .size = static_cast<uint32_t>(MAX_SPRITE_COUNT * sizeof(SpriteInstance))};

  pipeline.spriteDataTransferBuffer =
    SDL_CreateGPUTransferBuffer(gpuDevice, &transferBufferInfo);

  // Create data buffer
  constexpr SDL_GPUBufferCreateInfo bufferCreateInfo = {
    .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    .size = static_cast<uint32_t>(MAX_SPRITE_COUNT * sizeof(SpriteInstance))};

  pipeline.spriteDataBuffer = SDL_CreateGPUBuffer(gpuDevice, &bufferCreateInfo);

  // Record size for rendering
  pipeline.spriteDrawListSize = spriteDrawList.size();
  // END SPRITE RENDER PIPELINE ////////////////////////////

  // Create text render pipeline
  create_text_render_pipeline(renderer, canvas, &pipeline);

  return pipeline;
}

RendererData Renderer::createRenderer(const Window *window, const Canvas *canvas)
{
  RendererData renderer;

  renderer.internals.sdlWindowPtr = window->ptr;

  // Create GPU device
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

  return renderer;
}

void Renderer::draw(const Window *window)
{
  // Setup camera matrix
  const Rect windowBounds = getWindowBounds(window);

  const glm::mat4 cameraMatrix =
    glm::orthoZO(0.0f, static_cast<float>(windowBounds.width),
                 static_cast<float>(windowBounds.height), 0.0f, -1000.0f, 1000.0f);

  // Get command buffer
  auto *gpuDevice = window->renderer.internals.gpuDevice;
  auto *windowPtr = window->renderer.internals.sdlWindowPtr;

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

  if (swapchainTexture != nullptr) {
    auto &drawPipeline = window->renderer.drawPipeline;

    // Upload sprite instances
    const auto spriteDrawList = record_sprite_draw_list(&window->canvas);
    
    if (spriteDrawList.size() > MAX_SPRITE_COUNT) {
      UI_LOG_MSG("Warning: Sprite count (%zu) exceeds MAX_SPRITE_COUNT (%u). Clamping.",
                 spriteDrawList.size(), MAX_SPRITE_COUNT);
    }
    const size_t spriteCountToRender = std::min(spriteDrawList.size(), static_cast<size_t>(MAX_SPRITE_COUNT));
    
    const auto spriteDataPtr = static_cast<SpriteInstance *>(
      SDL_MapGPUTransferBuffer(gpuDevice, drawPipeline.spriteDataTransferBuffer, true));

    for (size_t i = 0; i < spriteCountToRender; i++) {
      spriteDataPtr[i] = spriteDrawList[i];
    }

    SDL_UnmapGPUTransferBuffer(gpuDevice, drawPipeline.spriteDataTransferBuffer);

    // Upload font glyph instances
    const auto textDrawList = record_glyph_draw_list(&window->canvas);
    
    if (textDrawList.size() > MAX_GLYPH_COUNT) {
      UI_LOG_MSG("Warning: Glyph count (%zu) exceeds MAX_GLYPH_COUNT (%u). Clamping.",
                 textDrawList.size(), MAX_GLYPH_COUNT);
    }
    const size_t glyphCountToRender = std::min(textDrawList.size(), static_cast<size_t>(MAX_GLYPH_COUNT));
    
    const auto textDataPtr = static_cast<FontGlyphInstance *>(
      SDL_MapGPUTransferBuffer(gpuDevice, drawPipeline.textTransferBuffer, true));

    for (size_t i = 0; i < glyphCountToRender; i++) {
      textDataPtr[i] = textDrawList[i];
    }

    SDL_UnmapGPUTransferBuffer(gpuDevice, drawPipeline.textTransferBuffer);

    // Upload sprite instance data
    SDL_GPUCopyPass *spriteCopyPass = SDL_BeginGPUCopyPass(cmd);

    const SDL_GPUTransferBufferLocation spriteTransferBufferLocation = {
      .transfer_buffer = drawPipeline.spriteDataTransferBuffer, .offset = 0};

    const SDL_GPUBufferRegion spriteBufferRegion = {
      .buffer = drawPipeline.spriteDataBuffer,
      .offset = 0,
      .size = static_cast<uint32_t>(spriteCountToRender * sizeof(SpriteInstance))};

    SDL_UploadToGPUBuffer(spriteCopyPass, &spriteTransferBufferLocation,
                          &spriteBufferRegion, true);


    SDL_EndGPUCopyPass(spriteCopyPass);

    // Upload text instance data
    SDL_GPUCopyPass *fontCopyPass = SDL_BeginGPUCopyPass(cmd);

    const SDL_GPUTransferBufferLocation textTransferBufferLocation = {
      .transfer_buffer = drawPipeline.textTransferBuffer, .offset = 0};

    const SDL_GPUBufferRegion textBufferRegion = {
      .buffer = drawPipeline.textBuffer,
      .offset = 0,
      .size = static_cast<uint32_t>(glyphCountToRender * sizeof(FontGlyphInstance))};

    SDL_UploadToGPUBuffer(fontCopyPass, &textTransferBufferLocation, &textBufferRegion,
                          true);

    SDL_EndGPUCopyPass(fontCopyPass);

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

    SDL_PushGPUVertexUniformData(cmd, 0, &cameraMatrix, sizeof(glm::mat4));

    // SPRITE RENDERING ////////////////////////
    SDL_BindGPUGraphicsPipeline(renderPass, drawPipeline.spriteDataPipeline);
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &drawPipeline.spriteDataBuffer, 1);
    SDL_DrawGPUPrimitives(renderPass, spriteCountToRender * 6, 1, 0, 0);
    // END SPRITE RENDERING ////////////////////////

    // TEXT RENDERING ////////////////////////
    SDL_BindGPUGraphicsPipeline(renderPass, drawPipeline.textPipeline);
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &drawPipeline.textBuffer, 1);

    const SDL_GPUTextureSamplerBinding fontAtlasTexBinding = {
      .texture = drawPipeline.fontAtlasTexture,
      .sampler = drawPipeline.fontAtlasSampler,
    };
    SDL_BindGPUFragmentSamplers(renderPass, 0, &fontAtlasTexBinding, 1);

    SDL_DrawGPUPrimitives(renderPass, glyphCountToRender * 6, 1, 0, 0);
    // END TEXT RENDERING ////////////////////////

    SDL_EndGPURenderPass(renderPass);
  }

  SDL_SubmitGPUCommandBuffer(cmd);
}
