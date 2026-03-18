Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
  float2 texCoord : TEXCOORD0;
  float4 color : TEXCOORD1;
};

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
  const float texSize = 216; // Hard-coded from msdf-atlas-gen settings

  float2 unitRange = float2(pxRange, pxRange) / float2(texSize, texSize);
  float2 screenTexSize = float2(1.0, 1.0) / sqrt(sqr(ddx(texCoord)) +
    sqr(ddy(texCoord)));

  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float4 main(Input input) : SV_Target0
{
  float3 msdf = Texture.Sample(Sampler, input.texCoord).rgb;
  float sd = median(msdf.r, msdf.g, msdf.b);

  float screenPxDistance = screenPxRange(input.texCoord) * (sd - 0.5);
  float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

  return float4(input.color.rgb, input.color.a * opacity);
}
