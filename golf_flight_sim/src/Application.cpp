#include "Application.h"
#include "../lib/SDL2_gfx/SDL2_gfxPrimitives.h"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_sdl.h"
#include "../lib/imgui/imgui_impl_sdlrenderer.h"
#include "./Physics/coefficients.h"
#include "./Physics/constants.h"
#include "./Physics/force.h"
#include "./math/trig.h"
#include "./math/unit_conversion.h"
#include "./misc/string_operations.h"
#include "./tracy/tracy/Tracy.hpp"
#include "Graphics.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>

uint32_t Application::window_width;
uint32_t Application::window_height;

bool Application::display_forces = false;

float get_spin_rate(float spin_rate, float time) {
  return spin_rate * std::expf(-time / SPIN_DECAY_RATE);
}

void Application::draw_primitives() {

  ZoneScoped; // for tracy

  // TODO: Write functions to convert world space coordinates into window space
  // coordinates. This is all getting a bit messy.

  SDL_SetRenderDrawColor(renderer, 5, 98, 99, 255);
  SDL_RenderClear(renderer);

  // Draw the ground
  const Uint32 ground_color = 0xFF395912;
  const Uint32 windowL_max_dimension_yards = 350;

  const Sint16 groundL_x1 = 0;
  const Sint16 groundL_x2 = static_cast<Sint16>(window_width * 0.75f);
  const Sint16 groundL_y1 = static_cast<Sint16>(window_height);
  Sint16 groundL_y2 = static_cast<Sint16>(window_height - window_height / 3);

  const Sint16 windowborderL = groundL_x2 - 4;
  Sint16 windowborderR = groundL_x2 + 4;

  const Uint16 windowL_length = groundL_x2 - groundL_x1;
  const float windowL_pixels_per_yard =
      static_cast<float>(windowL_length)
      / static_cast<float>(windowL_max_dimension_yards);

  boxColor(renderer, groundL_x1, groundL_y1, groundL_x2, groundL_y2,
           ground_color);

  const Uint32 windowR_max_dimension_yards = 350;

  const Sint16 groundR_x1 = groundL_x2;
  const Sint16 groundR_x2 = static_cast<Sint16>(window_width);
  const Sint16 groundR_y1 = 0;
  const Sint16 groundR_y2 = static_cast<Sint16>(window_height);

  const Uint16 windowR_length = groundR_x2 - groundR_x1;
  const float windowR_pixels_per_yard =
      static_cast<float>(window_height)
      / static_cast<float>(windowR_max_dimension_yards);

  boxColor(renderer, groundR_x1, groundR_y1, groundR_x2, groundR_y2,
           ground_color);

  // Draw all the balls in the scene
  const Uint32 ball_color = 0xFFFFFFFF;
  const Sint16 ball_radius = 4;

  for (auto &ball : balls) {

    ZoneNamedN(ball_draw_scope, "Ball Draw Routine", true); // for tracy

    Sint16 ballh_window_x = static_cast<Sint16>(
        (ball->position.x * windowL_pixels_per_yard) + 20.0f);
    Sint16 ballh_window_y = static_cast<Sint16>(
        static_cast<float>(window_height
                           - (ball->position.z * windowL_pixels_per_yard))
        - (static_cast<float>(window_height) / 3.0f));

    if (ballh_window_x - ball_radius < windowborderL) {
      filledCircleColor(renderer, ballh_window_x, ballh_window_y, ball_radius,
                        ball_color);
    }

    Uint16 windowR_center =
        static_cast<Sint16>(window_width) - (windowR_length / 2);

    Sint16 ballv_window_x = static_cast<Sint16>(
        -(ball->position.y * windowR_pixels_per_yard) + windowR_center);
    Sint16 ballv_window_y = static_cast<Sint16>(
        window_height - (ball->position.x * windowR_pixels_per_yard) - 20.0f);

    if (ballv_window_x + ball_radius > windowborderR) {
      filledCircleColor(renderer, ballv_window_x, ballv_window_y, ball_radius,
                        ball_color);
    }

    if (display_forces) {

      ZoneNamedN(display_forces_scope, "Display Forces Routine", true); // for tracy

      // Draw the forces acting on the ball

      float velocity_squared = ball->velocity.dot(ball->velocity);

      if (velocity_squared > MIN_ROLL_VELOCITY_SQUARED) {

        // Velocity
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x + ball->velocity.x * windowL_pixels_per_yard,
                 ballh_window_y - ball->velocity.z * windowL_pixels_per_yard),
            0xFFFF0000);

        Graphics::draw_arrow(
            renderer, vec2(ballv_window_x, ballv_window_y),
            vec2(ballv_window_x - ball->velocity.y * windowR_pixels_per_yard,
                 ballv_window_y - ball->velocity.x * windowR_pixels_per_yard),
            0xFFFF0000);

        // Acceleration
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x + ball->acceleration.x * windowL_pixels_per_yard,
                 ballh_window_y - ball->acceleration.z * windowL_pixels_per_yard),
            0xFF0000FF);

        Graphics::draw_arrow(
            renderer, vec2(ballv_window_x, ballv_window_y),
            vec2(ballv_window_x - ball->acceleration.y * windowR_pixels_per_yard,
                 ballv_window_y - ball->acceleration.x * windowR_pixels_per_yard),
            0xFF0000FF);

        // Gravity
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x,
                 ballh_window_y + GRAVITY * windowL_pixels_per_yard),
            0xFFFFFF00);

      }

      if (!ball->is_rolling) {

        // Wind
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x + ball->wind_force.x * windowL_pixels_per_yard,
                 ballh_window_y),
            0xFF00FF00);

        Graphics::draw_arrow(
            renderer, vec2(ballv_window_x, ballv_window_y),
            vec2(ballv_window_x - ball->wind_force.y * windowR_pixels_per_yard,
                 ballv_window_y - ball->wind_force.x * windowR_pixels_per_yard),
            0xFF00FF00);

        // Lift
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x
                     + ball->lift_force.x * windowL_pixels_per_yard * INV_BALL_MASS,
                 ballh_window_y
                     - ball->lift_force.z * windowL_pixels_per_yard
                           * INV_BALL_MASS),
            0xFF00FFFF);

        Graphics::draw_arrow(
            renderer, vec2(ballv_window_x, ballv_window_y),
            vec2(ballv_window_x
                     - ball->lift_force.y * windowR_pixels_per_yard * INV_BALL_MASS,
                 ballv_window_y
                     - ball->lift_force.x * windowR_pixels_per_yard * INV_BALL_MASS),
            0xFF00FFFF);

        // Drag
        Graphics::draw_arrow(
            renderer, vec2(ballh_window_x, ballh_window_y),
            vec2(ballh_window_x
                     + ball->drag_force.x * windowL_pixels_per_yard * INV_BALL_MASS,
                 ballh_window_y
                     - ball->drag_force.z * windowL_pixels_per_yard * INV_BALL_MASS),
            0xFFFF00FF);

        Graphics::draw_arrow(
            renderer, vec2(ballv_window_x, ballv_window_y),
            vec2(ballv_window_x
                     - ball->drag_force.y * windowR_pixels_per_yard * INV_BALL_MASS,
                 ballv_window_y
                     - ball->drag_force.x * windowR_pixels_per_yard * INV_BALL_MASS),
            0xFFFF00FF);

      }

    }

  }

  // Draw the border between the top and horizontal view
  boxColor(renderer, windowborderL, 0, windowborderR,
           static_cast<Sint16>(window_height), 0xFF183211);

  // Draw all the text labels
  // Update the counters
  text_strings[0]->text =
      "FPS: " + string_ops::float_to_string_formatted(current_fps, 1);
  text_strings[1]->text = "Number of balls: " + std::to_string(balls.size());

  // Add a space in front to keep the wind text offset and centered when the
  // counter goes below 10 mph.
  if (wind->speed < 9.5f) {
    text_strings[3]->text =
        " " + string_ops::float_to_string_formatted(wind->speed, 0) + " MPH";
  } else {
    text_strings[3]->text =
        string_ops::float_to_string_formatted(wind->speed, 0) + " MPH";
  }

  for (auto &text : text_strings) {

    // TODO: This isn't necessary at the moment, but this routine is by far the
    // slowest part of the entire game loop right now. In the future when
    // maintaining performance becomes more important, we can speed this up by
    // prerendering all the text as textures and displaying them on the screen
    // that way. For the changing numbers on the frame count and wind meter, we
    // can solve this by having each glyph stored as its own individual texture,
    // then arranging them appropriately at render time with a series of draw
    // calls.

    ZoneNamedN(ball_draw_scope, "Render Text Routine", true); // for tracy

    // Render the text
    SDL_Surface *surface = TTF_RenderText_Solid(
        asset_store->get_font(text->asset_id), text->text.c_str(), text->color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    int text_width = 0;
    int text_height = 0;

    SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);

    SDL_Rect dst_rect = {static_cast<int>(text->position.x),
                         static_cast<int>(text->position.y), text_width,
                         text_height};

    SDL_RenderCopy(renderer, texture, nullptr, &dst_rect);

    SDL_DestroyTexture(texture);

  }

  // Draw the wind arrow
  auto &wind_arrow = textures[0];

  SDL_RenderCopyEx(renderer, asset_store->get_texture(wind_arrow->asset_id),
                   nullptr, &wind_arrow->rect, rad_to_deg(-wind->direction),
                   nullptr, SDL_FLIP_NONE);

}

void Application::draw_imgui_gui() {

  ZoneScoped; // for tracy

  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // NOTE: Don't set the window size here. The scroll bars will break otherwise.
  ImGui::SetNextWindowPos(ImVec2(0, 0));

  if (ImGui::Begin("Configure Launch Parameters", NULL,
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoBackground
                       | ImGuiWindowFlags_NoTitleBar)) {

    // Initialize the launch parameters
    static float launch_speed_mph = 167.0f;
    static float launch_angle_deg = 10.9f;
    static float launch_heading_deg = 0.0f;
    static float launch_spin_rate = 2600.0f;
    static float spin_axis_deg = 0.0f;
    static float wind_speed_mph = 0.0f;
    static float wind_heading_deg = 0.0f;

    ImGui::Text("Launch Conditions");
    ImGui::Spacing();

    ImGui::SliderFloat("Launch Speed (mph)", &launch_speed_mph, 10, 300);
    ImGui::SliderFloat("Launch Angle (deg)", &launch_angle_deg, 0, 40);
    ImGui::SliderFloat("Launch Heading (deg)", &launch_heading_deg, -45, 45);
    ImGui::SliderFloat("Spin Rate (rpm)", &launch_spin_rate, 0, 20000);
    ImGui::SliderFloat("Spin Axis (deg)", &spin_axis_deg, -90, 90);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Wind Settings");
    ImGui::Spacing();

    // Adjust the wind vector in real time based on these fields
    ImGui::SliderFloat("Wind Speed (mph)", &wind->speed, 0, 30);
    ImGui::SliderAngle("Wind Heading (deg)", &wind->direction, 0, 360);
    ImGui::Checkbox("Use logarithmic wind model", &wind->log_wind);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Misc.");
    ImGui::Spacing();

    ImGui::Checkbox("Show forces", &display_forces);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Launch Ball")) {

      /*
        NOTE:
        On my machine at least the game starts to lag after shooting 1000 or
        so balls, so be careful!
      */

      // Convert the units of the launch parameters
      float launch_speed = mph_to_ms(launch_speed_mph);
      float launch_angle = deg_to_rad(launch_angle_deg);
      float launch_heading = deg_to_rad(launch_heading_deg) * -1.0f;
      float spin_axis_2d = deg_to_rad(spin_axis_deg);

      // Set the initial position and velocity vectors
      float tee_height = 0.0381f; // in meters (1.5 inches)
      auto ball_position = vec3(0.0, 0.0, tee_height);
      auto ball_velocity =
          vec3(launch_speed * cosf(launch_angle) * cosf(launch_heading),
               launch_speed * cosf(launch_angle) * sinf(launch_heading),
               launch_speed * sinf(launch_angle));

      // Calculate the 3D axis of rotation based on the launch heading and the
      // spin axis angle.
      auto rotation_axis =
          vec3(cosf(spin_axis_2d) * sinf(launch_heading),
               -cosf(spin_axis_2d) * cosf(launch_heading), spin_axis_2d);

      // Create a new ball and add it to the vector of balls
      std::unique_ptr<Ball> ball = std::make_unique<Ball>(
          ball_position, ball_velocity, rotation_axis, launch_spin_rate);

      balls.push_back(std::move(ball));

    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Balls")) {
      balls.clear();
    }

    // ImGui::Separator();
    // ImGui::Spacing();
    // ImGui::Separator();

    // if (ImGui::CollapsingHeader("Advanced Settings", 0)) {

    //  ImGui::SliderFloat("Ground Firmness", &GROUND_FIRMNESS, 0, 1);
    //  ImGui::SliderFloat("Coefficient of Friction", &FRICTION_ROLL, 0, 1);

    //}
  }

  ImGui::End();

  ImGui::SetNextWindowSize(ImVec2(310, 206));
  ImGui::SetNextWindowPos(ImVec2(406, 0));
  ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));

  if (ImGui::Begin("Ball Info", NULL,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                       | ImGuiWindowFlags_NoCollapse
                       | ImGuiWindowFlags_NoBackground
                       | ImGuiWindowFlags_NoTitleBar)) {

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    if (ImGui::BeginTable("ball_info", 1)) {

      for (int obj_i = 0; obj_i < balls.size(); obj_i++) {

        // Use object uid as identifier. Most commonly you could also use the
        // object pointer as a base ID.
        ImGui::PushID(obj_i);

        // Text and Tree nodes are less high than framed widgets, using
        // AlignTextToFramePadding() we add vertical spacing to make the tree
        // lines equal high.
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        bool node_open = ImGui::TreeNode("Ball", "%s %u", "Ball", obj_i + 1);

        if (node_open) {

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Position: %s", balls[obj_i]->position.to_str().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Velocity: %s", balls[obj_i]->velocity.to_str().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Acceleration: %s",
                      balls[obj_i]->acceleration.to_str().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Spin Rate: %s", string_ops::float_to_string_formatted(
                                           balls[obj_i]->current_spin_rate, 2)
                                           .c_str());

          ImGui::TreePop();

        }

        ImGui::PopID();

      }

      ImGui::EndTable();

    }

    ImGui::PopStyleVar();

    ImGui::End();

  }

  if (display_forces) {

    // Toggle the forces key
    ImGui::SetNextWindowSize(ImVec2(114, 135));
    ImGui::SetNextWindowPos(ImVec2(406, 206));

    if (ImGui::Begin("Forces", NULL,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                         | ImGuiWindowFlags_NoCollapse
                         | ImGuiWindowFlags_NoBackground
                         | ImGuiWindowFlags_NoTitleBar)) {

      ImGui::Text("Forces:");
      ImGui::Spacing();

      ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "  Velocity");
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "  Acceleration");
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "  Gravity");
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "  Wind");
      ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "  Lift");
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "  Drag");

    }

    ImGui::End();

  }

  //ImGui::ShowDemoWindow();

  ImGui::Render();
  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

}

Application::Application() {

  ZoneScoped; // for tracy

  std::cout << "Application constructor called."
            << "\n";
  is_running = false;
  asset_store = std::make_unique<AssetStore>();

}

Application::~Application() {

  ZoneScoped; // for tracy

  std::cout << "Application destructor called."
            << "\n";
}

void Application::initialize() {

  ZoneScoped; // for tracy

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {

    std::cerr << "Error initializing SDL."
              << "\n";
    return;

  }

  if (TTF_Init() != 0) {

    std::cerr << "Error initializing SDL_ttf."
              << "\n";
    return;

  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  window_width = 1280;
  window_height = 720;

  // Set the game update rate and FPS equal to the refresh rate of the monitor.
  seconds_per_frame = 1.0f / display_mode.refresh_rate;
  // seconds_per_frame = 1.0f / 100.0f;

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

  // Initialize the imgui context
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);

  is_running = true;

}

void Application::process_input() {

  ZoneScoped; // for tracy

  SDL_Event event;

  while (SDL_PollEvent(&event)) {

    // ImGui SDL input
    ImGui_ImplSDL2_ProcessEvent(&event);
    ImGuiIO &io = ImGui::GetIO();
    int mouse_x, mouse_y;
    const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    io.MousePos =
        ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

    // Handling core SDL events (keyboard movement, closing the window etc.)
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

  ZoneScoped; // for tracy

  // Set the initial wind conditions
  float wind_heading_deg = 0.0;
  float wind_speed_mph = 15.0;
  float wind_heading = deg_to_rad(wind_heading_deg);
  // Set to false to use the uniform wind profile instead of logarithmic
  bool log_wind = true;

  wind = std::make_unique<Wind>(wind_speed_mph, wind_heading, log_wind);

  int font_size = 12;

  // Add fonts and textures to the asset store
  asset_store->add_texture(renderer, "arrow", "./assets/textures/arrow.png");
  asset_store->add_font("pico8", "./assets/fonts/pico8.ttf", font_size);

  // Create the wind arrow
  int arrow_size = 35;
  int arrow_window_border_offset = 16;

  std::shared_ptr<Texture> arrow = std::make_shared<Texture>(
      "arrow",
      vec2(static_cast<float>(window_width - arrow_window_border_offset
                              - arrow_size - 15),
           static_cast<float>(arrow_window_border_offset)),
      35, 35);

  textures.push_back(arrow);

  // Create the UI text
  float text_y = static_cast<float>(window_height - font_size - 5);
  SDL_Color green = {0, 255, 0};

  std::unique_ptr<Text> fps_counter = std::make_unique<Text>(
      vec2(5.0, text_y), "FPS: " + std::to_string(current_fps), "pico8", green);

  std::unique_ptr<Text> num_balls_counter = std::make_unique<Text>(
      vec2(5.0, text_y - (font_size + 5)),
      "Number of balls: " + std::to_string(balls.size()), "pico8", green);

  std::unique_ptr<Text> program_title_label =
      std::make_unique<Text>(vec2((window_width / 2.0f) - 112, text_y),
                             "GOLF FLIGHT SIMULATOR 1.0", "pico8", green);

  std::string wind_meter_text =
      string_ops::float_to_string_formatted(wind->speed, 0) + " MPH";

  // Add a space in front to keep the text offset and centered when the counter
  // goes below 10 mph.
  if (wind->speed < 9.5f) {
    wind_meter_text = " " + wind_meter_text;
  }

  std::unique_ptr<Text> wind_meter = std::make_unique<Text>(
      vec2(window_width - 85.0f,
           static_cast<float>(arrow_window_border_offset + arrow_size + 5)),
      wind_meter_text, "pico8", green);

  text_strings.push_back(std::move(fps_counter));
  text_strings.push_back(std::move(num_balls_counter));
  text_strings.push_back(std::move(program_title_label));
  text_strings.push_back(std::move(wind_meter));

}

void Application::update() {

  ZoneScoped; // for tracy

  // Update the position of all balls in the scene
  for (auto &ball : balls) {

    // TODO: Resolve the collision between the ball and the ground in a better
    // way

    /* 
      Flight subroutine
      Calculates the trajectory of the ball through the air
    */

    if ((ball->position.z >= 0.0f) && (ball->is_rolling == false)) {

      // Calculates the wind force based off whether we are using the log wind
      // model or not.
      ball->wind_force = get_wind_force(wind, ball->position.z);

      // The ball's effective velocity, or "air speed" vector is determined by
      // taking the difference between the instantaneous velocity vector and the
      // wind vector.
      vec3 air_speed = ball->velocity - ball->wind_force;

      ball->current_spin_rate =
          get_spin_rate(ball->launch_spin_rate, ball->elapsed_time);

      // The coefficients of lift and drag are determined by the ball's speed
      // and spin rate. We take the square of the velocity vector here since we
      // don't need to to get the raw speed, which would involve an expensive
      // sqrt function.
      float air_speed_squared = air_speed.dot(air_speed);
      std::pair<float, float> coefficients = get_drag_and_lift_coefficients(
          air_speed_squared, ball->current_spin_rate);

      float drag_coefficient = coefficients.first;
      float lift_coefficient = coefficients.second;

      ball->lift_force =
          get_lift_force(air_speed, ball->rotation_axis, lift_coefficient);
      ball->drag_force = get_drag_force(air_speed, drag_coefficient);

      ball->sum_forces = ball->lift_force + ball->drag_force + BALL_WEIGHT;

      ball->integrate(seconds_per_frame);

      if ((ball->velocity.z < 0.0f) && (ball->max_height_set == false)) {
        ball->max_height = ball->position.z;
        ball->max_height_set = true;
      }

      ball->elapsed_time += seconds_per_frame;

    }

    /*
      Ground subroutine
      Covers the interactions between the ball and the ground when bouncing and rolling.
    */ 

    if (ball->position.z <= 0.0f) {

      ball->position.z = 0.0f;

      // End the bounce subroutine and start the roll subroutine if the max
      // height from the previous flight part was less than the specified
      // minimum bounce height of 5 mm.
      if (ball->max_height < MIN_BOUNCE_HEIGHT) {

        ball->is_rolling = true;
        ball->acceleration.zero();
        ball->wind_force.zero();
        ball->lift_force.zero();
        ball->drag_force.zero();
        ball->position.z = 0.0f;
        ball->velocity.z = 0.0f;

        // TODO: Compute the force of gravity tangential and normal to the local
        // terrain surface.
        // Calculate the friction on the ball from the surface of the green.
        float velocity_squared = ball->velocity.dot(ball->velocity);

        if (velocity_squared > MIN_ROLL_VELOCITY_SQUARED) {

          vec3 friction = get_friction_force(ball->velocity);
          ball->sum_forces = friction;

          ball->integrate(seconds_per_frame);
          velocity_squared = ball->velocity.dot(ball->velocity);

        } else {

          ball->velocity.zero();
          ball->acceleration.zero();
          ball->current_spin_rate = 0.0;

        }

      } else {

        /*
          We can simplify the collision of the ball bouncing against the ground
          to a 2D equation by defining a new frame of reference upon ground
          impact, where the x unit vector points along the direction of the
          velocity vector, the y unit vector points along the normal vector of
          the surface that the ball is colliding against, and the z vector
          perpendicular to unit vectors x and y.
        */

        // Calculate the transformation matrix for the new ground frame of
        // reference here.
        auto y_unit = vec3(0.0, 0.0, 1.0);
        y_unit /= norm(y_unit);
        auto z_unit = vec3(ball->velocity.cross(y_unit));
        z_unit /= norm(z_unit);
        auto x_unit = vec3(y_unit.cross(z_unit));

        // Calculate the new 2D velocity vector with respect to the local ground
        // frame
        float velocity_ground_x = ball->velocity.dot(x_unit);
        float velocity_ground_y = ball->velocity.dot(y_unit);

        // Gross but it works. TODO: Learn the minutae of floating point
        // comparisons.
        assert(static_cast<int>(ball->velocity.dot(z_unit)) == 0);

        //float normal_force = std::abs(velocity_ground_y);

        // Calculate the angular velocity of the ball with respect to the
        // ground.
        ball->current_spin_rate =
            get_spin_rate(ball->launch_spin_rate, ball->elapsed_time);
        float angular_velocity_ground_x = rpm_to_rad_s(ball->current_spin_rate)
                                          * ball->rotation_axis.dot(x_unit);
        float angular_velocity_ground_y = rpm_to_rad_s(ball->current_spin_rate)
                                          * ball->rotation_axis.dot(y_unit);
        float angular_velocity_ground_z = rpm_to_rad_s(ball->current_spin_rate)
                                          * ball->rotation_axis.dot(z_unit);

        /*
          When the ball hits the ground, it tends to penetrate into the ground
          and slip across it. These forces act as both a linear and angular
          impulse on the ball, changing its linear and angular velocity
          differently than how one would expect from a normal inelastic
          collision. We can represent these interactions by thinking of the
          collision as if the ball were colliding with a plane angled at an
          angle theta_c above the angle of the surface the ball is colliding
          against.
        */

        float ball_speed = norm(ball->velocity);

        float theta_c =
            GROUND_FIRMNESS * ball_speed
            * fast_atan(std::abs(velocity_ground_x / velocity_ground_y));

        // Use theta c to transform to the ball velocity vector components from
        // the x-y frame to the x'-y' frame, where the x' axis is equal to a
        // surface inclined at angle theta_c from the original surface.

        float velocity_ground_x_transformed =
            velocity_ground_x * cosf(theta_c)
            + velocity_ground_y * sinf(theta_c);
        float velocity_ground_y_transformed =
            -(velocity_ground_x * sinf(theta_c))
            + velocity_ground_y * cosf(theta_c);

        float normal_force_transformed =
            std::abs(velocity_ground_y_transformed);

        float restitution =
            get_coefficient_of_restitution(normal_force_transformed);

        // Calculate the critical values of the coefficient of friction for the
        // x'-y' and z-y' planes. If the coefficient of friction for the surface
        // exceeds these values, the ball will roll instead of slide through
        // impact for that particular plane.
        float mu_cz = (-2.0f / 7.0f)
                      * (velocity_ground_x_transformed
                         + (RADIUS * angular_velocity_ground_z))
                      / (normal_force_transformed * (1.0f + restitution));
        float mu_cx = (2.0f / 7.0f) * (RADIUS * angular_velocity_ground_x)
                      / (normal_force_transformed * (1.0f + restitution));

        // Calculate the linear and angular velocity in the x'-y' plane.
        if (FRICTION < mu_cz) {

          // Linear and angular velocity in the x'-y' plane after sliding
          velocity_ground_x_transformed -=
              FRICTION * (normal_force_transformed * (1 + restitution));

          velocity_ground_y_transformed =
              restitution * normal_force_transformed;

          angular_velocity_ground_z -=
              ((5.0f * FRICTION) / (2.0f * RADIUS))
              * (normal_force_transformed * (1.0f + restitution));

        } else {

          // Linear and angular velocity in the x'-y' plane after rolling
          velocity_ground_x_transformed =
              (1.0f / 7.0f)
              * (5.0f * velocity_ground_x_transformed
                 - (2.0f * RADIUS * angular_velocity_ground_z));
          velocity_ground_y_transformed =
              restitution * normal_force_transformed;

          angular_velocity_ground_z = -(velocity_ground_x_transformed / RADIUS);

        }

        // Calculate the linear and angular velocity in the z-y' plane.
        float velocity_ground_z;

        if (FRICTION < mu_cx) {

          // Linear and angular velocity in the z-y' plane after sliding
          velocity_ground_z =
              -FRICTION * (normal_force_transformed * (1 + restitution));

          angular_velocity_ground_x -=
              ((5.0f * FRICTION) / (2.0f * RADIUS))
              * (normal_force_transformed * (1.0f + restitution));

        } else {

          // Linear and angular velocity in the z-y' plane after rolling
          velocity_ground_z =
              (-2.0f / 7.0f) * RADIUS * angular_velocity_ground_x;

          angular_velocity_ground_x = -(velocity_ground_z / RADIUS);

        }


        // TODO: When the ball backspins after landing, the angle it spins back
        // at is in the opposite direction from what you would expect.
        // Investigate this.

        // Transform the x and y components from the x'-y' frame back to the
        // original ground frame.
        velocity_ground_x = velocity_ground_x_transformed * cosf(theta_c)
                            - velocity_ground_y_transformed * sinf(theta_c);
        velocity_ground_y = velocity_ground_x_transformed * sinf(theta_c)
                            + velocity_ground_y_transformed * cosf(theta_c);

        // Convert the components for the ground frame back to the world frame.
        // The flight subroutine will be called once again with these values as
        // the new parameters.
        ball->velocity = (velocity_ground_x * x_unit)
                         + (velocity_ground_y * y_unit)
                         + (velocity_ground_z * z_unit);

        vec3 angular_velocity = (angular_velocity_ground_x * x_unit)
                                + (angular_velocity_ground_y * y_unit)
                                + (angular_velocity_ground_z * z_unit);

        ball->launch_spin_rate = rad_s_to_rpm(norm(angular_velocity));

        ball->rotation_axis = angular_velocity / norm(angular_velocity);

        ball->max_height_set = false;

      }

    }

  }

}

void Application::render() {

  ZoneScoped; // for tracy

  draw_primitives();
  draw_imgui_gui();

  ZoneNamedN(SDL_RenderPresent_scope, "SDL_RenderPresent", true);
  SDL_RenderPresent(renderer);

}

void Application::run() {

  setup();

  current_fps = 1.0f / seconds_per_frame;

  while (is_running) {

    auto frame_start = std::chrono::high_resolution_clock::now();

    // Main game loop is here
    process_input();
    update();
    render();

    FrameMark; // for tracy

    auto frame_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> game_update_time = frame_end - frame_start;

    if (game_update_time.count() < seconds_per_frame) {

      ZoneNamedN(sleep_scope, "Sleep", true); // for tracy

      uint32_t ms_to_wait = static_cast<uint32_t>(
          (seconds_per_frame - game_update_time.count()) * 1000.0f);

      SDL_Delay(ms_to_wait);

    }

    std::chrono::duration<float> elapsed_time_for_frame =
        std::chrono::high_resolution_clock::now() - frame_start;
    current_fps = 1.0f / elapsed_time_for_frame.count();

  }

}

void Application::destroy() {

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

}