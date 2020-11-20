#include "Interpolater.hpp"

using namespace myrisa;

// interpolation routines are from Lomas Modules src/dsp/Interpolation

// https://github.com/chen0040/cpp-spline
float Interpolater::BSpline(const float P0, const float P1, const float P2, const float P3, float u)
{
    float point;
    point = u * u * u * ((-1) * P0 + 3 * P1 - 3 * P2 + P3) / 6;
    point += u * u * (3 * P0 - 6 * P1 + 3 * P2) / 6;
    point += u * ((-3) * P0 + 3 * P2) / 6;
    point += (P0 + 4 * P1 + P2) / 6;

    return point;
}

float Interpolater::Hermite4pt3oX(float x0, float x1, float x2, float x3, float t) {
    float c0 = x1;
    float c1 = .5F * (x2 - x0);
    float c2 = x0 - (2.5F * x1) + (2 * x2) - (.5F * x3);
    float c3 = (.5F * (x3 - x0)) + (1.5F * (x1 - x2));
    return (((((c3 * t) + c2) * t) + c1) * t) + c0;
}

/** Float precission position `x`.
The array at `p` must be at least length `floor(x) + 2`.
*/
float Interpolater::interpolateLinearD(float* data, float position) {
    int x1 = floor(position);
    float t = position - x1;
    return rack::crossfade(data[x1], data[x1+1], t);
}

/** The array at `p` must be at least length `floor(x) + 3`.
 */
float Interpolater::InterpolateHermite(float* data, float position) {
    int x1 = floor(position);
    float t = position - x1;
    return Hermite4pt3oX(data[x1 - 1], data[x1], data[x1 + 1], data[x1 + 2], t);
}

/* The array at `p` must be at least length `floor(x) + 3`. */
float Interpolater::interpolateBSpline(const float *data, float position) {
  int x1 = floor(position);
  float t = position - x1;
  return BSpline(data[x1 - 1], data[x1], data[x1 + 1], data[x1 + 2], t);
}

/** interpolates an array. Warps. */
float Interpolater::interpolateLineard(float *data, float position,
                                       int data_len) {
  int x1 = floor(position);
  int x2 = (x1 + 1) % data_len;
  float t = position - x1;
  return rack::crossfade(data[x1], data[x2], t);
}

/** interpolates an array. Warps. */
float Interpolater::InterpolateHermite(float* data, float position, int data_len) {
    int x1 = (int)floor(position);
    int x0 = (x1 < 1) ? data_len - 1 : x1 - 1;
    int x2 = (x1 + 1) % data_len;
    int x3 = (x2 + 1) % data_len;
    float t = position - x1;
    return Hermite4pt3oX(data[x0], data[x1], data[x2], data[x3], t);
}

/** interpolates an array. Warps. */
float Interpolater::interpolateBSpline(const float *data, float position,
                                       int data_len) {
  int x1 = (int)floor(position);
  int x0 = (x1 < 1) ? data_len - 1 : x1 - 1;
  int x2 = (x1 + 1) % data_len;
  int x3 = (x2 + 1) % data_len;
  float t = position - x1;
  return BSpline(data[x0], data[x1], data[x2], data[x3], t);
}

/** interpolates an array. Warps. */
float Interpolater::interpolate(Method interpolation_method,
                                vector<float> samples, float position) {
  switch (interpolation_method) {
  case NONE:
    return samples[floor(position)];
    break;
  case LINEAR:
    return interpolateLinearD(samples.data(), position);
    break;
  case HERMITE:
    return InterpolateHermite(samples.data(), position,
                              samples.size());
    break;
  case BSPLINE:
    return interpolateBSpline(samples.data(), position);
    break;
  default:
    return samples[floor(position)];
  }
}
