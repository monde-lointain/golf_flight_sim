#pragma once

#include <cstdint>

struct GameWindow {

  int16_t width;
  int16_t height;
  int16_t x;
  int16_t y;

  GameWindow(int16_t width, int16_t height, int16_t x, int16_t y);
  ~GameWindow() = default;

};
