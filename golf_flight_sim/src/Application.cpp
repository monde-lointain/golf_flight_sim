#include "Application.h"
#include <SDL.h>
#include "../lib/SDL2_gfx/SDL2_gfxPrimitives.h"
#include <chrono>
#include <iostream>

inline float get_hires_clock_in_seconds() {
  uint64_t perf_counter = SDL_GetPerformanceCounter();
  uint64_t perf_frequency = SDL_GetPerformanceFrequency();
  float current_time_seconds =
      static_cast<float>(perf_counter) / static_cast<float>(perf_frequency);
  return current_time_seconds;
}

Application::Application() {
  is_running = false;
  std::cout << "Application constructor called."
            << "\n";
}

Application::~Application() {
  std::cout << "Application destructor called."
            << "\n";
}

void Application::initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Error initializing SDL."
              << "\n";
    return;
  }
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = 1280;
  window_height = 720;
  monitor_refresh_rate = display_mode.refresh_rate;
  window =
      SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width, window_height, SDL_WINDOW_BORDERLESS);
  if (!window) {
    std::cerr << "Error creating SDL window."
              << "\n";
    return;
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    std::cerr << "Error creating SDL renderer."
              << "\n";
    return;
  }

  is_running = true;
}

void Application::process_input() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
        is_running = false;
      break;
    }
  }
}

void Application::setup() {
  /* Initialize the ball and launch parameters here */
}

void Application::update() {}

void Application::render() {
  SDL_SetRenderDrawColor(renderer, 5, 98, 99, 255);
  SDL_RenderClear(renderer);

  filledCircleColor(renderer, 200, 200, 40, 0xFFFFFFFF);

  SDL_RenderPresent(renderer);
}

void Application::run() {
  setup();
  while (is_running) {
    process_input();
    update();
    render();
  }
}

void Application::destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}