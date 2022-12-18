#pragma once

#include "../math/vec2.h"
#include <SDL.h>
#include <string>

struct Texture {

  std::string asset_id;
  SDL_Rect rect;
  float rotation;

  Texture(std::string asset_id, vec2 position, int width, int height);
  ~Texture() = default;

};
