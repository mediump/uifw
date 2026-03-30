#pragma once

namespace ui {

class MathUtils
{
public:
  /**
   * @brief Remaps a value from one range to another.
   *
   * This function performs a linear interpolation to map a value from an input
   * range [in_start, in_end] to an output range [out_start, out_end].
   * The value is not clamped by default, allowing for out-of-range inputs.
   *
   * @tparam T The numeric type (e.g., int, float, double).
   * @param value The input value to remap.
   * @param in_start The start of the input range.
   * @param in_end The end of the input range.
   * @param out_start The start of the output range.
   * @param out_end The end of the output range.
   * @return The remapped value in the output range's coordinate system.
   */
  template <typename T>
  static T remapRange(T value, T inStart, T inEnd, T outStart, T outEnd)
  {
    if (inStart == inEnd) {
      // Handle division by zero edge case
      return outStart;
    }

    double scale = static_cast<double>(outEnd - outStart) / (inEnd - inStart);
    double remappedValue = outStart + (value - inStart) * scale;

    return static_cast<T>(remappedValue);
  }
};

} // namespace ui
