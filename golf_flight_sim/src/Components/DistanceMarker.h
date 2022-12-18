#pragma once

#include "../math/vec2.h"
#include "Text.h"
#include <memory>
#include <vector>

struct DistanceMarker {

  int num_markers;
  std::vector<std::shared_ptr<Text>> marker_text;
  vec2 offset;
  float spacing;
  int frequency;

  DistanceMarker(int num_markers, vec2 position, vec2 offset, float spacing,
                 int frequency, const std::string &asset_id,
                 const SDL_Color &color);
  ~DistanceMarker() = default;

};
