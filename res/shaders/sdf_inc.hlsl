/* === SDF Helper Functions (Include) === */

// SDF representation of a sharp rectangle
float sdBox(float2 local_p, float2 sdf_bounds)
{
  float2 d = abs(local_p) - sdf_bounds;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// SDF representation of a rounded rectangle rectangle
float sdRoundedBox(float2 local_p, float2 sdf_bounds, float4 r)
{
  float2 s = float2(local_p.x >= 0.0f ? 1.0f : -1.0f, local_p.y >= 0.0f ? 1.0f : -1.0f);
  float2 p_abs = abs(local_p);

  // Get the radius for the current corner quadrant
  float radius = (s.x > 0.0) ?
    ((s.y > 0.0) ? r.y : r.z) : // TR, BR
    ((s.y > 0.0) ? r.x : r.w);  // TL, BL

  // If this corner's radius is zero, use sharp box SDF for that corner
  if (radius <= 0.0) {
    return sdBox(local_p, sdf_bounds);
  }

  float2 q = p_abs - sdf_bounds + radius;
  return min(max(q.x, q.y), 0.0) + length(max(q, 0.0f)) - radius;
}