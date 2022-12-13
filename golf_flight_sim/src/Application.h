#pragma once

#include "./AssetStore/AssetStore.h"
#include "./Components/Text.h"
#include "./Components/Ball.h"
#include "./Components/Wind.h"
#include <SDL.h>
#include <memory>
#include <vector>

const float PIXELS_PER_METER = 4.0f;

class Application {
private:
  uint32_t window_width;
  uint32_t window_height;
  float seconds_per_frame;
  float current_fps;

  bool is_running;
  SDL_Window *window;
  SDL_Renderer *renderer;

  std::unique_ptr<AssetStore> asset_store;

  std::vector<std::unique_ptr<Text>> text_strings;
  std::vector<std::unique_ptr<Ball>> balls;
  // array of strings: text
  // vector of balls
  // fps

  //std::unique_ptr<Ball> ball;
  std::unique_ptr<Wind> wind;
  //float ground_height;


public:
  Application();
  ~Application();

  void initialize();
  void run();
  void setup();
  void process_input();
  void update();
  void render();
  void destroy();

};
