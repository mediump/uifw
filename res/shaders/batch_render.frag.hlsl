struct Input
{
  float4 color : TEXCOORD0;
  float4 borderRadius : TEXCOORD1;
  float4 spriteBounds : TEXCOORD2;
  float3 fragmentPosition : TEXCOORD3;
};

// ---- Corner states ---- //
static const int CORNER_NONE = -1;
static const int CORNER_TOP_LEFT = 0;
static const int CORNER_TOP_RIGHT = 1;
static const int CORNER_BOTTOM_RIGHT = 2;
static const int CORNER_BOTTOM_LEFT = 3;
// ---------------------- //

// Detect if the current fragment is in a corner. Return integer defining
//  which corner the fragment is currently in.
//
//  -1 = Not in corner
//   0 = Top-left
//   1 = Top-right
//   2 = Bottom-left
//   3 = Bottom-right
bool is_in_corner(float3 pt, float4 spriteBounds, float4 borderRadius) {
  float2 halfSize = spriteBounds.zw * 0.5f;
  float2 center = spriteBounds.xy + halfSize;
  float2 localP = pt.xy - center;
  
  float2 s = float2(localP.x >= 0.0f ? 1.0f : -1.0f, 
                    localP.y >= 0.0f ? 1.0f : -1.0f);

  int ix = (s.x >= 0.0f) ? 1 : 0;
  int iy = (s.y >= 0.0f) ? 0 : 2;
  int idx = ix + iy;

  float4 mask = float4(idx == 0, idx == 1, idx == 2, idx == 3);
  float cornerR = dot(borderRadius, mask);

  float2 q = abs(localP);
  float2 threshold = halfSize - cornerR;
  bool inCornerSquare = (q.x > threshold.x && q.y > threshold.y);
  
  if (inCornerSquare) {
    float2 delta = q - threshold;
    return dot(delta, delta) <= (cornerR * cornerR);
  }
  
  return true;
}

float4 main(Input input) : SV_Target0
{
  if (!is_in_corner(input.fragmentPosition, input.spriteBounds, input.borderRadius)) {
    discard;
  }
  
  return input.color;
}
