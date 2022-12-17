#include "DistanceMarker.h"
#include "../misc/string_operations.h"
#include "../math/unit_conversion.h"

DistanceMarker::DistanceMarker(int num_markers, vec2 position, vec2 offset,
                               float spacing, int frequency,
                               const std::string &asset_id,
                               const SDL_Color &color) {

  // Only create text labels for every x markers
  for (int i = frequency - 1; i < num_markers / frequency; i++) {

    auto text_label = std::make_shared<Text>(
        position,
        string_ops::float_to_string_formatted(
            m_to_yd(static_cast<float>(i * frequency) * spacing), 0),
        asset_id, color);
    this->marker_text.push_back(text_label);

  }

  this->num_markers = num_markers;
  this->offset = offset;
  this->spacing = spacing;
  this->frequency = frequency;

}
