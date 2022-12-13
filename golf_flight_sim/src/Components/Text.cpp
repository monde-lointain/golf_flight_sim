#include "Text.h"

Text::Text(vec2 position, const std::string &text, const std::string &asset_id,
           const SDL_Color &color) {

  this->position = position;
  this->text = text;
  this->asset_id = asset_id;
  this->color = color;

}
