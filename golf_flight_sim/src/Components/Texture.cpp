#include "Texture.h"

Texture::Texture(std::string asset_id, vec2 position, int width, int height) {

  this->asset_id = asset_id;
  this->rect = {static_cast<int>(position.x), static_cast<int>(position.y),
                width, height};
  this->rotation = 0.0f;
}