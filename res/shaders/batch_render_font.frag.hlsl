#include "sdf_inc.hlsl"

Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
  float2 texCoord : TEXCOORD0;
  float2 localPos : TEXCOORD1;
  float4 bounds : TEXCOORD2;
  float4 color : TEXCOORD3;
  float4 parentBounds : TEXCOORD4;
  float4 parentRadius : TEXCOORD5;
};

/* --- CLIPPING --- */

float getClip(float2 pLocal, float4 spriteBounds, float4 parentBounds, float4 parentRadii)
{
  // Convert local position to world space
  float2 spriteCenter = spriteBounds.xy + (spriteBounds.zw * 0.5f);
  float2 pixelWorldPos = spriteCenter + pLocal;

  // Convert world position to parent space
  float2 parentHalf = parentBounds.zw * 0.5f;
  float2 parentCenter = parentBounds.xy + parentHalf;
  float2 pParentSpace = pixelWorldPos - parentCenter;

  return sdRoundedBox(pParentSpace, parentHalf, parentRadii);
}

/* ---------------- */

/* --- FONT RENDERING --- */

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float2 sqr(float2 x)
{
  return x * x;
}

float screenPxRange(float2 texCoord)
{
  const float pxRange = 4; // 4px defined in msdf-atlas-gen settings
  const float texSize = 512; // Hard-coded from msdf-atlas-gen settings

  float2 unitRange = float2(pxRange, pxRange) / float2(texSize, texSize);
  float2 screenTexSize = float2(1.0, 1.0) / sqrt(sqr(ddx(texCoord)) +
    sqr(ddy(texCoord)));

  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

/* ---------------------- */

float4 main(Input input) : SV_Target0
{
  // Clipping early rejection
  float dClip = getClip(input.localPos, 
                        input.bounds, 
                        input.parentBounds, 
                        input.parentRadius);
  clip(-dClip);

  float3 msdf = Texture.Sample(Sampler, input.texCoord).rgb;
  float sd = median(msdf.r, msdf.g, msdf.b);

  float screenPxDistance = screenPxRange(input.texCoord) * (sd - 0.5);
  float opacity = clamp(screenPxDistance + 0.49, 0.0, 1.0);

  return float4(input.color.rgb, input.color.a * opacity);
}
