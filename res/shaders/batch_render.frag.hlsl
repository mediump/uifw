struct Input
{
  float4 color : TEXCOORD0;
  float4 borderColor : TEXCOORD1;
  float4 borderRadius : TEXCOORD2;
  float4 borderWidths : TEXCOORD3;
  float2 localPos : TEXCOORD4;
  float2 size : TEXCOORD5;
};

float sdRoundedBox(float2 p, float2 b, float4 r)
{
    float2 s = float2(p.x >= 0.0f ? 1.0f : -1.0f, p.y >= 0.0f ? 1.0f : -1.0f);
    float radius = (s.x > 0.0) ? 
      ((s.y > 0.0) ? r.y : r.z) : // TR, BR
      ((s.y > 0.0) ? r.x : r.w);  // TL, BL

    float2 q = abs(p) - b + radius;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0f)) - radius;
}

float4 main(Input input) : SV_Target0
{
  float2 p = input.localPos;
  float4 r = input.borderRadius;
  float4 b = input.borderWidths;

  // Outline
  float dOuter = sdRoundedBox(p, input.size * 0.5f, r);

  // Fill
  float2 innerOffset = float2((b.w - b.y) * 0.5f, (b.z - b.x) * 0.5f);
  float2 innerHalfSize = (input.size - float2(b.w + b.y, b.x + b.z)) * 0.5f;
  
  float4 innerR = max(0.0f, r - float4(max(b.w, b.x), max(b.y, b.x), max(b.y, b.z), max(b.w, b.z)));
  float dInner = sdRoundedBox(p - innerOffset, innerHalfSize, innerR);

  // Masking
  float unit = fwidth(dOuter);
  float outerMask = 1.0f - smoothstep(-unit, 0.0f, dOuter);
  float innerMask = 1.0f - smoothstep(-unit, 0.0f, dInner);
  float borderMask = max(0.0, outerMask - innerMask);

  // Composition
  float4 fillColor = input.color * innerMask;
  float4 borderColor = input.borderColor * borderMask;
  
  float4 final = fillColor + borderColor;
  return float4(final.rgb, final.a * outerMask);
}
