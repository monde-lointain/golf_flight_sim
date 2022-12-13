#pragma once

#include "../Physics/vec2.h"
#include <SDL.h>
#include <string>

struct Text {

  vec2 position;
  std::string text;
  std::string asset_id;
  SDL_Color color;

  Text(vec2 position, const std::string &text, const std::string &asset_id,
       const SDL_Color &color);
  ~Text() = default;

};