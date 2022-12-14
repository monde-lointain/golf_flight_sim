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
#include "./misc/colors.h"
#include "./misc/string_operations.h"
#include "./tracy/tracy/Tracy.hpp"
#include "Graphics.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>

Uint16 Application::window_width;
Uint16 Application::window_height;

bool Application::display_forces = false;
bool Application::display_trajectories = false;

float get_spin_rate(float spin_rate, float time) {
  return spin_rate * std::expf(-time / SPIN_DECAY_RATE);
}

void Application::draw_primitives() {

  ZoneScoped; // for tracy

  // WORLD COORDINATES SYSTEM SHOULD ALWAYS BE IN METERS!!! ONLY USE FEET AND
  // YARDS FOR IMGUI STUFF!!

  SDL_SetRenderDrawColor(renderer, 5, 98, 99, 255);
  SDL_RenderClear(renderer);

  // Calculate the coordinates of the windows in world space in meters.
  const float windows_world_min_x = -45.72f;
  const float windows_world_max_x = 350.0f;

  const float windowL_pixels_per_meter =
      static_cast<float>(windowL->width)
      / (windows_world_max_x - windows_world_min_x);

  const float windowR_pixels_per_meter =
      static_cast<float>(windowR->height)
      / static_cast<float>(windows_world_max_x - windows_world_min_x);

  const Sint16 windowR_center =
      static_cast<Sint16>(windowR->x) + (windowR->width / 2);

  // Set the border width between the windows
  const Sint16 windowborderL = windowL->width - 4;
  const Sint16 windowborderR = windowL->width + 4;

  // Draw the ground
  const Uint32 ground_color = 0xFF395912;

  const Sint16 groundL_x1 = 0;
  const Sint16 groundL_x2 = windowborderL;
  const Sint16 groundL_y1 = window_height;
  const Sint16 groundL_y2 = groundL_y1 - groundL_y1 / 3;

  boxColor(renderer, groundL_x1, groundL_y1, groundL_x2, groundL_y2,
           ground_color);

  const Sint16 groundR_x1 = windowborderR;
  const Sint16 groundR_x2 = windowborderR + windowR->width;
  const Sint16 groundR_y1 = 0;
  const Sint16 groundR_y2 = windowR->height;

  boxColor(renderer, groundR_x1, groundR_y1, groundR_x2, groundR_y2,
           ground_color);

  // Draw the distance markers

  for (int i = 0; i < distance_markers->num_markers; i++) {

    ZoneNamedN(draw_distance_markers_scope, "Draw Distance Markers Routine", true); // for tracy

    // Convert the distance markers from world coordinates into windowL
    // coordinates.
    Uint32 marker_color = colors::GREEN;

    // Draw the markers in the left window (along the world x axis only)
    float marker_x_pos_windowL =
        static_cast<float>((distance_markers->spacing * i)
                           - windows_world_min_x)
        * windowL_pixels_per_meter;
    float marker_y_pos_windowL = groundL_y2;
    vec2 windowL_marker_start =
        vec2(marker_x_pos_windowL, static_cast<float>(groundL_y2 - 2));
    vec2 windowL_marker_end =
        vec2(marker_x_pos_windowL, static_cast<float>(groundL_y2 + 2));

    Graphics::draw_line(renderer, windowL_marker_start, windowL_marker_end,
                        colors::GREEN);

    // Draw the markers in the right window (along the world x and y axes)
    float marker_y_pos_windowR = static_cast<float>(
        windowR->height
        - ((distance_markers->spacing * i) - windows_world_min_x)
              * windowR_pixels_per_meter);
    float marker_x_pos_windowR = windowR_center;

    Graphics::draw_crosshair(renderer, marker_x_pos_windowR,
                             marker_y_pos_windowR, marker_color);

    if (i % distance_markers->frequency == 0 && i > 0) {

      ZoneNamedN(draw_distance_marker_labels_scope, "Draw Distance Marker Labels Routine", true); // for tracy

      // Draw the text labels for all the markers. Note that
      // NOTE: I feel like this should be WAY less complicated than it currently is, but I don't have any ideas for how to do it atm
      int j = ((i + 1) / distance_markers->frequency) - 1;

      auto &text_marker = distance_markers->marker_text[j];

      // Render the text
      SDL_Surface *surface =
          TTF_RenderText_Solid(asset_store->get_font(text_marker->asset_id),
                               text_marker->text.c_str(), text_marker->color);
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);

      int text_width = 0;
      int text_height = 0;

      SDL_QueryTexture(texture, nullptr, nullptr, &text_width, &text_height);

      // Draw the text to the left window
      SDL_Rect dst_rect_windowL = {
          static_cast<int>(marker_x_pos_windowL + distance_markers->offset.x),
          static_cast<int>(marker_y_pos_windowL + distance_markers->offset.y),
          text_width, text_height};

      SDL_RenderCopy(renderer, texture, nullptr, &dst_rect_windowL);

      // Draw the text to the right window
      SDL_Rect dst_rect_windowR = {
          static_cast<int>(marker_x_pos_windowR + distance_markers->offset.x),
          static_cast<int>(marker_y_pos_windowR + distance_markers->offset.y),
          text_width, text_height};

      SDL_RenderCopy(renderer, texture, nullptr, &dst_rect_windowR);

      SDL_DestroyTexture(texture);

    }

    //int num_marker_rows = 3;
    //// If we ever want to make multiple rows of markers in the right window
    //for (int j = 1; j <= num_marker_rows; j++) {
    //  float marker_x_pos_windowR = static_cast<float>(windowR->height - ((marker_spacing_meters * i) - windows_world_min_x) * windowR_pixels_per_meter);
    //  Graphics::draw_crosshair(renderer, windowR->x + static_cast<float>((j * windowR->width) / 4), marker_x_pos_windowR, marker_color);
    //}

  }

  // Draw a line across the bottom of the right window to indicate 0 yards.
  float beginning_marker_y =
      windowR->height + (windows_world_min_x * windowR_pixels_per_meter);
  vec2 beginning_marker_start_point = vec2(windowborderR, beginning_marker_y);
  vec2 beginning_marker_end_point = vec2(window_width, beginning_marker_y);

  Graphics::draw_line(renderer, beginning_marker_start_point,
                      beginning_marker_end_point, colors::GREEN);

  // Draw all the balls in the scene
  const Uint32 ball_color = colors::WHITE;
  const Sint16 ball_radius = 4;

  for (auto &ball : balls) {

    ZoneNamedN(ball_draw_scope, "Ball Draw Routine", true); // for tracy

    // Calculate the screen coordinates of the ball for both the left and right
    // windows
    vec2 windowL_ball_coordinates = vec2(
        (ball->position.x - windows_world_min_x) * windowL_pixels_per_meter,
        (ball->position.z * windowL_pixels_per_meter * -1.0f)
            + static_cast<float>(groundL_y2));

    vec2 windowR_ball_coordinates =
        vec2(-(ball->position.y * windowR_pixels_per_meter)
                 + static_cast<float>(windowR_center),
             static_cast<float>(windowR->height)
                 - ((ball->position.x - windows_world_min_x)
                    * windowR_pixels_per_meter));

    // Draw the trajectories of all balls to the trajectories texture
    if (trajectories_texture == nullptr) {

      ZoneNamedN(display_trajectories_create_texture_scope,
                 "Display Trajectories Routine: Create new texture",
                 true); // for tracy

      // Create the texture to draw the traif it doesn't exist already
      trajectories_texture = SDL_CreateTexture(
          renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
          window_width, window_height);

      SDL_SetTextureBlendMode(trajectories_texture, SDL_BLENDMODE_BLEND);

    }

    // Draw to the texture, then immediately set the renderer target back to
    // the window
    SDL_SetRenderTarget(renderer, trajectories_texture);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Only draw a ball's trajectory if it resides within the window
    if (static_cast<Sint16>(windowL_ball_coordinates.x) < windowborderL) {
      SDL_RenderDrawPoint(renderer,
                          static_cast<Sint16>(windowL_ball_coordinates.x),
                          static_cast<Sint16>(windowL_ball_coordinates.y));
    }

    if (static_cast<Sint16>(windowR_ball_coordinates.x) > windowborderR) {
      SDL_RenderDrawPoint(renderer,
                          static_cast<Sint16>(windowR_ball_coordinates.x),
                          static_cast<Sint16>(windowR_ball_coordinates.y));
    }

    SDL_SetRenderTarget(renderer, nullptr);

    // Draw the balls to the screen
    if (windowL_ball_coordinates.x - ball_radius < windowborderL) {
      filledCircleColor(renderer,
                        static_cast<Sint16>(windowL_ball_coordinates.x),
                        static_cast<Sint16>(windowL_ball_coordinates.y),
                        ball_radius, ball_color);
    }

    if (windowR_ball_coordinates.x + ball_radius > windowborderR) {
      filledCircleColor(renderer,
                        static_cast<Sint16>(windowR_ball_coordinates.x),
                        static_cast<Sint16>(windowR_ball_coordinates.y),
                        ball_radius, ball_color);
    }

    if (display_forces) {

      float velocity_squared = ball->velocity.dot(ball->velocity);

      // Only draw the forces when the ball is in motion.
      if (velocity_squared > MIN_ROLL_VELOCITY_SQUARED) {

        // Draw all the forces acting on the ball

        // Velocity
        Graphics::draw_force_vector(
            renderer, ball->velocity, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::BLUE);

        // Acceleration
        Graphics::draw_force_vector(
            renderer, ball->acceleration, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::RED);

        // Gravity
        Graphics::draw_force_vector(
            renderer, GRAVITY_VEC, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::CYAN);

      }

      if (!ball->is_rolling) {

        // Wind
        Graphics::draw_force_vector(
            renderer, ball->wind_force, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::GREEN);

        vec3 lift_force_ms = ball->lift_force * INV_BALL_MASS;

        // Lift
        Graphics::draw_force_vector(
            renderer, lift_force_ms, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::YELLOW);

        vec3 drag_force_ms = ball->drag_force * INV_BALL_MASS;

        // Drag
        Graphics::draw_force_vector(
            renderer, drag_force_ms, windowL_ball_coordinates,
            windowR_ball_coordinates, windowL_pixels_per_meter,
            windowR_pixels_per_meter, ball_radius, windowborderL, windowborderR,
            colors::MAGENTA);

      }

    }

  }

  // Only render the trajectories texture to the screen when
  // display_trajectories is toggled on
  if (display_trajectories) {

    // Draw the positions of all the balls to the target texture, then
    // immediately go back to drawing to the window like normal
    SDL_RenderCopy(renderer, trajectories_texture, nullptr, nullptr);

    SDL_SetRenderTarget(renderer, nullptr);

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

  if (ImGui::Begin("Configure Launch Parameters", nullptr,
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

    ImGui::SameLine();

    ImGui::Checkbox("Toggle ball trajectories", &display_trajectories);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Launch Ball")) {

      /*
        NOTE:
        On my machine at least the game starts to lag after shooting 500 or
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

    if (ImGui::Button("Launch Volley")) {

      // Launches 11 balls instead of one

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

      for (int i = 1; i <= 5; i++) {

        spin_axis_2d = deg_to_rad(10.0f * static_cast<float>(i));
        rotation_axis =
            vec3(cosf(spin_axis_2d) * sinf(launch_heading),
                 -cosf(spin_axis_2d) * cosf(launch_heading), spin_axis_2d);
        auto rotation_axis_2 =
            vec3(rotation_axis.x, -rotation_axis.y, rotation_axis.z);

        std::unique_ptr<Ball> ball1 = std::make_unique<Ball>(
            ball_position, ball_velocity, rotation_axis, launch_spin_rate);
        std::unique_ptr<Ball> ball2 = std::make_unique<Ball>(
            ball_position, ball_velocity, -rotation_axis_2, launch_spin_rate);

        balls.push_back(std::move(ball1));
        balls.push_back(std::move(ball2));

      }

    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Balls")) {

      // Delete all the balls from the scene and remove the trajectories if they exist
      balls.clear();

      if (trajectories_texture) {

        // Fill the texture with blank pixels then immediately go back to
        // drawing to the window
        SDL_SetRenderTarget(renderer, trajectories_texture);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderTarget(renderer, NULL);

      }

    }

    //ImGui::Separator();
    //ImGui::Spacing();
    //ImGui::Separator();

    //if (ImGui::CollapsingHeader("Advanced Settings", 0)) {

    //  ImGui::SliderFloat("Ground Firmness", &GROUND_FIRMNESS, 0, 1);
    //  // ImGui::SliderFloat("Coefficient of Friction", &FRICTION_ROLL, 0, 1);
    //}

  }

  ImGui::End();

  ImGui::SetNextWindowSize(ImVec2(380, 206));
  ImGui::SetNextWindowPos(ImVec2(386, 0));
  ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));

  if (ImGui::Begin("Ball Info", nullptr,
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
          ImGui::Text("Position (yds): %s",
                      balls[obj_i]->position.to_str_in_yds().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Velocity (ft/s): %s",
                      balls[obj_i]->velocity.to_str_in_ft().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Acceleration (ft/s^2): %s",
                      balls[obj_i]->acceleration.to_str_in_ft().c_str());

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("Spin Rate (rpm): %s",
                      string_ops::float_to_string_formatted(
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
    ImGui::SetNextWindowPos(ImVec2(386, 206));

    if (ImGui::Begin("Forces", nullptr,
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

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Golf Flight Simulator 1.0",
                             "Error initializing SDL.", nullptr);
    return;

  }

  if (TTF_Init() != 0) {

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Golf Flight Simulator 1.0",
                             "Error initializing SDL_ttf.", nullptr);
    return;

  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  window_width = 1280;
  window_height = 720;

  // Set the game update rate and FPS equal to the refresh rate of the monitor.
  seconds_per_frame = 1.0f / display_mode.refresh_rate;
  // seconds_per_frame = 1.0f / 100.0f;

  window = SDL_CreateWindow("Golf Flight Simulator 1.0" , SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, window_width, window_height,
                            SDL_WINDOW_BORDERLESS);

  if (!window) {

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Golf Flight Simulator 1.0",
                             "Error creating SDL window.", nullptr);
    return;

  }

  renderer =
      SDL_CreateRenderer(window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                             | SDL_RENDERER_TARGETTEXTURE);

  if (!renderer) {

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Golf Flight Simulator 1.0",
                             "Error creating SDL renderer.", nullptr);
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
    // Disable saving the ini file since the window positions are always fixed anyways
    io.IniFilename = nullptr;
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

  // Create the right and left windows in the main window
  const Sint16 windowL_width = static_cast<Sint16>(static_cast<float>(window_width) * 0.75f);
  const Sint16 windowL_height = window_height;

  const Sint16 windowR_width = window_width - windowL_width;
  const Sint16 windowR_height = window_height;
  const Sint16 windowR_x = windowL_width;

  windowL = std::make_unique<GameWindow>(windowL_width, windowL_height, 0, 0);
  windowR = std::make_unique<GameWindow>(windowR_width, windowR_height, windowR_x, 0);

  // Set the initial wind conditions
  float wind_heading_deg = 0.0;
  float wind_speed_mph = 15.0;
  float wind_heading = deg_to_rad(wind_heading_deg);
  // Set to false to use the uniform wind profile instead of logarithmic
  bool log_wind = true;

  wind = std::make_unique<Wind>(wind_speed_mph, wind_heading, log_wind);

  int font_size_12 = 12;
  int font_size_5 = 5;

  // Add fonts and textures to the asset store
  asset_store->add_texture(renderer, "arrow", "./assets/textures/arrow.png");
  asset_store->add_font("pico8", "./assets/fonts/pico8.ttf", font_size_12);
  asset_store->add_font("pico8_5", "./assets/fonts/pico8.ttf", font_size_5);

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
  float text_y = static_cast<float>(window_height - font_size_12 - 5);
  SDL_Color green = {0, 255, 0};

  std::unique_ptr<Text> fps_counter = std::make_unique<Text>(
      vec2(5.0, text_y), "FPS: " + std::to_string(current_fps), "pico8", green);

  std::unique_ptr<Text> num_balls_counter = std::make_unique<Text>(
      vec2(5.0, text_y - (font_size_12 + 5)),
      "Number of balls: " + std::to_string(balls.size()), "pico8", green);

  std::unique_ptr<Text> program_title_label =
      std::make_unique<Text>(vec2((window_width / 2.0f) - 150, text_y),
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

  std::unique_ptr<Text> quit_button_msg = std::make_unique<Text>(
      vec2((window_width / 2.0f) - 107, text_y - (font_size_12 + 5)),
                             "PRESS ESC. TO QUIT", "pico8", green);

  text_strings.push_back(std::move(fps_counter));
  text_strings.push_back(std::move(num_balls_counter));
  text_strings.push_back(std::move(program_title_label));
  text_strings.push_back(std::move(wind_meter));
  text_strings.push_back(std::move(quit_button_msg));

  // Create the distance markers
  // TODO: Currently these are being stored in a vector, which I think is
  // probably unoptimal. Maybe there's a way to have them stored in an array
  // instead?
  int num_markers = 8;
  float marker_spacing_meters = 45.72f; // 50 yards
  vec2 marker_offset = vec2(5.0f, 5.0f);
  int markers_per_text_label = 2;

  distance_markers = std::make_unique<DistanceMarker>(
      num_markers, vec2(0.0f, 0.0f), marker_offset, marker_spacing_meters,
      markers_per_text_label, "pico8_5", green);

  // Create the texture to draw the ball trajectories on
  trajectories_texture = NULL;

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
        float ball_x_speed_ground = std::abs(velocity_ground_x);
        float ball_y_speed_ground = std::abs(velocity_ground_y);

        float theta_c;

        // Get rid of these when you're done investigating this.
        float angle =
            rad_to_deg(fast_atan(ball_y_speed_ground / ball_x_speed_ground));
        float angle2 =
            rad_to_deg(fast_atan(ball_x_speed_ground / ball_y_speed_ground));

        // I don't understand why I have to do this but it works. This seems to
        // produce good results for both the ball coming in at a steep angle and
        // a shallow one
        if (ball_x_speed_ground > ball_y_speed_ground) {
          theta_c =
              GROUND_FIRMNESS * ball_speed
              * fast_atan(ball_y_speed_ground / ball_x_speed_ground);
        } else {
          theta_c =
              GROUND_FIRMNESS * ball_speed
              * fast_atan(ball_x_speed_ground / ball_y_speed_ground);
        }

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
                      / (velocity_ground_y_transformed * (1.0f + restitution));
        float mu_cx = (2.0f / 7.0f) * (RADIUS * angular_velocity_ground_x)
                      / (velocity_ground_y_transformed * (1.0f + restitution));

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
          velocity_ground_z = FRICTION * (normal_force_transformed * (1 + restitution));

          angular_velocity_ground_x -=
              ((5.0f * FRICTION) / (2.0f * RADIUS))
              * (normal_force_transformed * (1.0f + restitution));

        } else {

          // Linear and angular velocity in the z-y' plane after rolling
          velocity_ground_z = (2.0f / 7.0f) * RADIUS * angular_velocity_ground_x;

          angular_velocity_ground_x = -(velocity_ground_z / RADIUS);

        }

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
  SDL_DestroyTexture(trajectories_texture);
  trajectories_texture = NULL;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  //TTF_Quit();
  SDL_Quit();

}