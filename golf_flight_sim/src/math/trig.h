#pragma once

#include <cmath>
#include "../Physics/constants.h"

inline float fast_atan(float x) {

  /* Fast approximation of atan using Horner's method. */

  float result;

  const float a1 =   0.9999991518f;
  const float a3 =  -0.3329188436f;
  const float a5 =   0.1993941203f;
  const float a7 =  -0.1387495263f;
  const float a9 =   0.0956061958f;
  const float a11 = -0.0548522460f;
  const float a13 =  0.0211605470f;
  const float a15 = -0.0038682211f;

  if (std::abs(x) < 1) {

    float x_sq = x * x;
    result = x * (a1 + x_sq * (a3 + x_sq * (a5 + x_sq * (a7 + x_sq * (a9 + x_sq * (a11 + x_sq * (a13 + x_sq * a15)))))));

  } else {

    float y = 1.0f / x;
    float y_sq = y * y;
    float atan_y = y * (a1 + y_sq * (a3 + y_sq * (a5 + y_sq * (a7 + y_sq * (a9 + y_sq * (a11 + y_sq * (a13 + y_sq * a15)))))));
    result = -atan_y + ((x < 0) ? -0.5f * PI : 0.5f * PI);

  }

  return result;

}

inline float fast_atan2(float y, float x) {

  float result = 0.0f;

  if (x == 0 || y == 0) {

    if (x != 0) {
      result = x > 0 ? 0.0f : PI;
    } else if (y != 0) {
      result = y > 0 ? 0.5f * PI : 1.5f * PI;
    }

  } else if (x > 0) {
    result = 2.0f * fast_atan(y / (hypotf(x, y) + x));
  } else {
    result = 2.0f * fast_atan((hypotf(x, y) - x) / y);
  }

  return result;

}