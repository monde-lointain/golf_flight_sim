#pragma once

#include "../Physics/vec2.h"

vec2 solve_y_linear(vec2 a, vec2 b, float x) {

  float dx = b.x - a.x;
  float dy = b.y - a.y;
  float slope = dy / dx;

  // Solve for y using the point slope formula
  vec2 result;
  result.x = x;
  result.y = (slope * x) - (slope * b.x) + b.y;

  return result;

}