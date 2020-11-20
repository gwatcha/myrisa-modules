// copied from Lomas Modules src/dsp/Interpolation

#pragma once

#include <rack.hpp>
#include <vector>

using namespace std;

namespace myrisa {

class Interpolater {
public:
  enum Method {
    NONE,
    LINEAR,
    HERMITE,
    BSPLINE,
  };

  static float interpolate(Method method, vector<float> samples, float index);

private:
  // https://github.com/chen0040/cpp-spline
  static float BSpline(const float P0, const float P1, const float P2, const float P3, float u);

  static float Hermite4pt3oX(float x0, float x1, float x2, float x3, float t);

  /** Static Float precision position `x`.
  The array at `p` must be at least length `floor(x) + 2`.
  */
  static float interpolateLinearD(float* data, float position);

  /** The array at `p` must be at least length `floor(x) + 3`.
  */
  static float InterpolateHermite(float* data, float position) ;

  /** The array at `p` must be at least length `floor(x) + 3`. */
  static float interpolateBSpline(const float* data, float position) ;

  /** interpolates an array. Warps. */
  static float interpolateLineard(float* data, float position, int data_len);

  /** interpolates an array. Warps. */
  static float InterpolateHermite(float* data, float position, int data_len);

  /** interpolates an array. Warps. */
  static float interpolateBSpline(const float* data, float position, int data_len);
};

} // namespace myrisa
