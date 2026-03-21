struct SpriteData
{
  float3 position;
  float rotation;
  float2 size;
  float2 _padding; // Padding variable to satisfy GLSL std140 layout requirement (extra 16 bytes)
  float4 color;
  float4 borderRadius;
};

struct Output
{
  float4 color : TEXCOORD0;
  float4 borderRadius : TEXCOORD1;
  float4 spriteBounds : TEXCOORD2;
  float3 fragmentPosition : TEXCOORD3;
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

    float c = cos(sprite.rotation);
    float s = sin(sprite.rotation);

    float2 coord = vertexPos[vert];
    coord *= sprite.size;

    float2x2 rotation = {c, s, -s, c};
    coord = mul(coord, rotation);

    float3 coordWithDepth = float3(coord + sprite.position.xy, sprite.position.z);

    Output output;

    output.position = mul(viewProjectionMatrix, float4(coordWithDepth, 1.0f));
    output.color = sprite.color;
    output.borderRadius = sprite.borderRadius;
    output.spriteBounds = float4(sprite.position.xy, sprite.size);
    output.fragmentPosition = coordWithDepth;

    return output;
}