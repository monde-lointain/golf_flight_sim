#pragma once

#include "./Physics/Ball.h"
#include "./Physics/Wind.h"
#include <SDL.h>
#include <memory>

const float PIXELS_PER_METER = 4.0f;

class Application {
private:
  uint32_t window_width;
  uint32_t window_height;
  float seconds_per_frame;
  float previous_frame_time = 0.0f;
  float elapsed_simulation_time = 0.0f;

  bool is_running;
  SDL_Window *window;
  SDL_Renderer *renderer;

  std::unique_ptr<Ball> ball;
  std::unique_ptr<Wind> wind;
  float ground_height;

public:
  Application() = default;
  ~Application() = default;

  void initialize();
  void run();
  void setup();
  void process_input();
  void update();
  void render();
  void destroy();
};
