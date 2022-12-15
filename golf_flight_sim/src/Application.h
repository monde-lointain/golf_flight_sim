#pragma once

#include "./AssetStore/AssetStore.h"
#include "./Components/Texture.h"
#include "./Components/Text.h"
#include "./Components/Ball.h"
#include "./Components/Wind.h"
#include <SDL.h>
#include <memory>
#include <vector>

const float PIXELS_PER_METER = 4.0f;

class Application {
private:
  float seconds_per_frame;
  float current_fps;

  bool is_running;
  SDL_Window *window;
  SDL_Renderer *renderer;

  std::unique_ptr<AssetStore> asset_store;

  std::vector<std::shared_ptr<Texture>> textures;
  std::vector<std::unique_ptr<Text>> text_strings;
  std::vector<std::unique_ptr<Ball>> balls;

  std::unique_ptr<Wind> wind;

  static bool display_forces;

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
  void draw_primitives();
  void draw_imgui_gui();

  static uint32_t window_width;
  static uint32_t window_height;

};
