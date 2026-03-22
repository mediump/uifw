struct Input
{
  float4 color : TEXCOORD0;
  float4 borderColor : TEXCOORD1;
  float4 borderRadius : TEXCOORD2;
  float4 borderWidths : TEXCOORD3;
  float2 localPos : TEXCOORD4;
  float2 size : TEXCOORD5;
  float4 parentBounds : TEXCOORD6;
  float4 parentRadius : TEXCOORD7;
};

float sdBox(float2 p, float2 b)
{
  float2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdRoundedBox(float2 p, float2 b, float4 r)
{
  float2 s = float2(p.x >= 0.0f ? 1.0f : -1.0f, p.y >= 0.0f ? 1.0f : -1.0f);
  float2 p_abs = abs(p);

  // Get the radius for the current corner quadrant
  float radius = (s.x > 0.0) ?
    ((s.y > 0.0) ? r.y : r.z) : // TR, BR
    ((s.y > 0.0) ? r.x : r.w);  // TL, BL

  // If this corner's radius is zero, use sharp box SDF for that corner
  if (radius <= 0.0) {
      float2 d = p_abs - b;
      return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
  }

  float2 q = p_abs - b + radius;
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0f)) - radius;
}

// Get the radius for the current pixel's corner quadrant
float getCornerRadius(float2 p, float4 r)
{
  float2 s = float2(p.x >= 0.0f ? 1.0f : -1.0f, p.y >= 0.0f ? 1.0f : -1.0f);
  return (s.x > 0.0) ?
    ((s.y > 0.0) ? r.y : r.z) : // TR, BR
    ((s.y > 0.0) ? r.x : r.w);  // TL, BL
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

  float outerRadius = getCornerRadius(p, r);
  float innerRadius = getCornerRadius(p - innerOffset, innerR);
  float effectiveRadius = min(outerRadius, innerRadius);
  
  float baseUnit = fwidth(dOuter);
  // Reduce AA width for sharp corners
  float aaWidth = (effectiveRadius <= 0.0) ? baseUnit * 0.25f : baseUnit;
  
  float outerMask = 1.0f - smoothstep(-aaWidth, 0.0f, dOuter);
  float innerMask = 1.0f - smoothstep(-aaWidth, 0.0f, dInner);
  float borderMask = max(0.0, outerMask - innerMask);

  // Composition
  float4 fillColor = input.color * innerMask;
  float4 borderColor = input.borderColor * borderMask;

  float4 final = fillColor + borderColor;
  return float4(final.rgb, final.a * outerMask);
}
