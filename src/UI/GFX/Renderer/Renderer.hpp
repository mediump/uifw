#pragma once

#include "RendererTypes.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "Utils.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <vector>

namespace ui {

UI_FORWARD_DECLARE_STRUCT(Canvas);
UI_FORWARD_DECLARE_STRUCT(WindowData);

struct SDLInternals
{
  SDL_Window *sdlWindowPtr = nullptr;
  SDL_GPUDevice *gpuDevice = nullptr;
};

// Pre-allocated storage for draw data to avoid per-frame allocations
struct DrawDataStorage
{
  std::vector<SpriteInstance> spriteInstances;
  std::vector<FontGlyphInstance> glyphInstances;
  
  // Track counts from last frame for dirty checking
  size_t lastSpriteCount = 0;
  size_t lastGlyphCount = 0;
  
  // Cache last window dimensions for camera matrix
  uint16_t lastWindowWidth = 0;
  uint16_t lastWindowHeight = 0;
  glm::mat4 cachedCameraMatrix = glm::mat4(1.0f);
  bool cameraMatrixValid = false;
};

struct DrawPipeline
{
  // Sprites
  SDL_GPUGraphicsPipeline *spriteDataPipeline = nullptr;
  SDL_GPUTransferBuffer *spriteDataTransferBuffer = nullptr;
  SDL_GPUBuffer *spriteDataBuffer = nullptr;
  size_t spriteDrawListSize = 0;
  // Text
  SDL_GPUGraphicsPipeline *textPipeline = nullptr;
  SDL_GPUSampler *fontAtlasSampler = nullptr;
  SDL_GPUTexture *fontAtlasTexture = nullptr;
  SDL_GPUTransferBuffer *textTransferBuffer = nullptr;
  SDL_GPUBuffer *textBuffer = nullptr;
  size_t textDrawListSize = 0;
};

struct RendererData
{
  SDLInternals internals;
  DrawPipeline drawPipeline;
  DrawDataStorage storage;  // Pre-allocated storage for draw data
};

class Renderer
{
public:
  static RendererData createRenderer(const WindowData *window, const Canvas *canvas,
                                     SDL_GPUDevice *gpuDevice);

  static void draw(WindowData *window);

private:
  static void pick_window_present_mode(const RendererData *renderer);

  static DrawPipeline create_draw_pipeline(const RendererData *renderer,
                                           const Canvas *canvas);

  static size_t record_sprite_draw_list(const WindowData *window,
                                        std::vector<SpriteInstance> &outInstances);

  static void create_text_render_pipeline(const RendererData *renderer,
                                          const Canvas *canvas,
                                          DrawPipeline *pipeline);

  static const glm::mat4& get_camera_matrix(DrawDataStorage &storage, 
                                            uint16_t windowWidth, 
                                            uint16_t windowHeight);

  // static void call_embedded_pre_render(const WindowData *window, const Canvas *canvas,
  //                                      SDL_GPUDevice *gpuDevice);                                           
};

} // namespace ui
