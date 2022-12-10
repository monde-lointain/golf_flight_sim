#pragma once

#include <SDL.h>

struct Application {
  uint32_t window_width{};
  uint32_t window_height{};
  int monitor_refresh_rate{};

  bool is_running;
  SDL_Window *window{};
  SDL_Renderer *renderer{};

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
