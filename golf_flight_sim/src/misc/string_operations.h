#pragma once

#include <iomanip>
#include <sstream>
#include <string>

namespace string_ops {

inline std::string float_to_string_formatted(float f, int precision) {

  std::stringstream stream;
  stream << std::fixed << std::setprecision(precision) << f;
  return stream.str();
}

} // namespace string_ops
