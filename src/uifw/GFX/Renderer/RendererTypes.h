#pragma once

#include "uifw/Core/Coordinates.h"
#include "uifw/Core/Utils/Memory.h"

#include <SDL3/SDL.h>

/* ---- RENDER PRIMITIVES ---- */

typedef struct
{
  ui_Vector3f position;
  ui_Color4f color;
} ui_Renderer_Vertex;

/* --------------------------- */

/* ---- DRAW PIPELINE ---- */

typedef enum
{
  DrawPipelineType_Sprite,
  DrawPipelineType_FontGlyph,
} ui_Renderer_DrawPipelineType;

typedef struct
{
  ui_Renderer_DrawPipelineType type;
  bool is_dirty;
  size_t size;

  SDL_GPUGraphicsPipeline *pipeline;
  SDL_GPUTransferBuffer *transfer_buffer;
  SDL_GPUBuffer *data_buffer;

  SDL_GPUSampler *texture_sampler;
  SDL_GPUTexture *texture;

  void *data;
} ui_Renderer_DrawPipeline;

/* ----------------------- */

/* ---- INSTANCES ---- */

typedef struct
{
  ui_Vector3f position;
  float rotation;
  ui_Vector2f size;
  UI_PAD_FLOAT(2);
  ui_Color4f color;
} ui_Renderer_SpriteInstance;

typedef struct
{
  ui_Vector3f position;
  UI_PAD_FLOAT(1);
  ui_Vector2f size;
  UI_PAD_FLOAT(2);
  ui_Vector4f texture_coords;
  ui_Color4f color;
} ui_Renderer_FontGlyphInstance;

/* ------------------- */

/* ---- RENDERER ---- */

typedef struct
{
  bool is_dirty;
  ui_Vector2i last_window_size;
  ui_Matrix4 view_matrix;
} ui_Renderer_CameraData;

typedef struct
{
  SDL_Window *window_ref;
  SDL_GPUDevice *gpu_device;
  ui_Renderer_CameraData camera_data;
  ui_Renderer_DrawPipeline *sprite_pipeline;
  ui_Renderer_DrawPipeline *font_pipeline;
} ui_Renderer;

/* ------------------ */