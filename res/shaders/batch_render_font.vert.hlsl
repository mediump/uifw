struct FontGlyphInstance
{
  float3 position;
  float _pad0;
  float2 size;
  float2 _pad1;
  float4 texCoords;
  float4 color;
  float4 parentBounds;
  float4 parentRadius;
};

struct Output
{
  float2 texCoord : TEXCOORD0;
  float2 localPos : TEXCOORD1;
  float4 bounds : TEXCOORD2;
  float4 color : TEXCOORD3;
  float4 parentBounds : TEXCOORD4;
  float4 parentRadius : TEXCOORD5;
  float4 position : SV_Position;
};

StructuredBuffer<FontGlyphInstance> DataBuffer : register(t0, space0);

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

  FontGlyphInstance sprite = DataBuffer[spriteIndex];

  float2 texCoord[4] = {
    {sprite.texCoords.x, sprite.texCoords.y}, // u0, v0
    {sprite.texCoords.z, sprite.texCoords.y}, // u1, v0
    {sprite.texCoords.x, sprite.texCoords.w}, // u0, v1
    {sprite.texCoords.z, sprite.texCoords.w}  // u1, v1
  };

  float2 coord = vertexPos[vert];
  coord *= sprite.size;

  float3 coordWithDepth = float3(coord + sprite.position.xy, sprite.position.z);

  Output output;

  output.position = mul(viewProjectionMatrix, float4(coordWithDepth, 1.0f));
  output.localPos = coord;
  output.bounds = float4(sprite.position.xy, sprite.size);
  output.texCoord = texCoord[vert];
  output.color = sprite.color;
  output.parentBounds = sprite.parentBounds;
  output.parentRadius = sprite.parentRadius;

  return output;
}