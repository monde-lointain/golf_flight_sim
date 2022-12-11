#pragma once

#include "vec3.h"

struct Wind {
  vec3 wind;
  bool log_wind;

  Wind(vec3 wind, bool log_wind);
  ~Wind() = default;
};
