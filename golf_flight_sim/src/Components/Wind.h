#pragma once

#include "../math/vec3.h"

struct Wind {

  float speed;
  float direction;
  bool log_wind;

  Wind(float speed, float direction, bool log_wind);
  ~Wind() = default;

};
