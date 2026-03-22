struct SpriteData
{
  float3 position;
  float rotation;
  float2 size;
  float2 _padding; // Padding variable to satisfy GLSL std140 layout requirement (extra 16 bytes)
  float4 color;
  float4 borderRadius;
  float4 borderColor;
  float4 borderWidths;
  float4 parentBounds;
  float4 parentRadius;
};

struct Output
{
  float4 color : TEXCOORD0;
  float4 borderColor : TEXCOORD1;
  float4 borderRadius : TEXCOORD2;
  float4 borderWidths : TEXCOORD3;
  float2 localPos : TEXCOORD4;
  float2 size : TEXCOORD5;
  float4 parentBounds : TEXCOORD6;
  float4 parentRadius : TEXCOORD7;
  float4 position : SV_Position;
};

// Mutable buffer of SpriteData structs. Each SpriteData struct
// contains the necessary data to render the element: position,
// rotation, scale, and color
StructuredBuffer<SpriteData> DataBuffer : register(t0, space0);

cbuffer UniformBlock : register(b0, space1)
{
  float4x4 viewProjectionMatrix : packoffset(c0);
}

static const uint triangleIndices[6] = {0, 1, 2, 3, 2, 1};
static const float2 vertexPos[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
};

Output main(uint id : SV_VertexID)
{
  uint spriteIndex = id / 6;
  uint vert = triangleIndices[id % 6];
  SpriteData sprite = DataBuffer[spriteIndex];

  float2 coord = vertexPos[vert];
  float2 localPos = coord - 0.5f;

  float c = cos(sprite.rotation);
  float s = sin(sprite.rotation);
  float2x2 rotation = {c, s, -s, c};

  float2 rotatedCoord = mul(localPos * sprite.size, rotation);
  float3 worldPos = float3(rotatedCoord + sprite.position.xy + (sprite.size * 0.5f), sprite.position.z);

  Output output;
  output.position = mul(viewProjectionMatrix, float4(worldPos, 1.0f));
  output.color = sprite.color;
  output.borderColor = sprite.borderColor;
  output.borderRadius = sprite.borderRadius;
  output.borderWidths = sprite.borderWidths;
  output.localPos = localPos * sprite.size;
  output.size = sprite.size;
  output.parentBounds = sprite.parentBounds;
  output.parentRadius = sprite.parentRadius;

  return output;
}