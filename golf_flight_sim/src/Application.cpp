#include "Application.h"
#include "../lib/SDL2_gfx/SDL2_gfxPrimitives.h"
#include "./Physics/coefficients.h"
#include "./Physics/constants.h"
#include "./Physics/force.h"
#include "./Physics/vec3.h"
#include "./math/trig.h"
#include "./math/unit_conversion.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

float get_spin_rate(float spin_rate, float time) {
  return spin_rate * std::expf(-time / SPIN_DECAY_RATE);
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

  // Set the game update rate and FPS equal to the refresh rate of the monitor.
  seconds_per_frame = 1.0f / static_cast<float>(display_mode.refresh_rate);
  //seconds_per_frame = 1.0f / 100.0f;

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

  // Initialize the launch parameters
  //   float launch_speed_mph = 160.0;
  float launch_angle_deg = 10.0;
  float launch_heading_deg = 1.1f;
  float launch_spin_rate = 2000.0;
  float spin_axis_deg = 7.0;
  //   float wind_speed_mph = 0.0;
  float wind_heading_deg = 190.0;
  // Set to false to use the uniform wind profile instead of logarithmic
  bool log_wind = true;

  // Convert the units of the launch parameters
  //   float launch_speed = mph_to_ms(launch_speed_mph);
  float launch_speed = 75.0;
  float launch_angle = deg_to_rad(launch_angle_deg);
  float launch_heading = deg_to_rad(launch_heading_deg);
  float spin_axis_2d = deg_to_rad(spin_axis_deg);
  //   float wind_speed = mph_to_ms(wind_speed_mph);
  float wind_speed = 15.0; // in m/s
  float wind_heading = deg_to_rad(wind_heading_deg);

  // Set the initial position and velocity vectors
  float tee_height = 0.0381f; // in meters (1.5 inches)
  auto ball_position = vec3(0.0, 0.0, tee_height);
  auto ball_velocity =
      vec3(launch_speed * cosf(launch_angle) * cosf(launch_heading),
           launch_speed * cosf(launch_angle) * sinf(launch_heading),
           launch_speed * sinf(launch_angle));

  // Calculate the 3D axis of rotation based on the launch heading and the spin
  // axis angle.
  auto rotation_axis =
      vec3(cosf(spin_axis_2d) * sinf(launch_heading),
           -cosf(spin_axis_2d) * cosf(launch_heading), spin_axis_2d);

  ball = std::make_unique<Ball>(ball_position, ball_velocity, rotation_axis,
                                launch_spin_rate);

  ground_height = 0.0f;

  // The wind z-component will always be assumed to be zero. That is, the wind
  // will always be assumed to be blowing horizontally, instead of up or down
  // towards the ground.
  auto wind_force = vec3(wind_speed * cosf(wind_heading),
                         wind_speed * sinf(wind_heading), 0.0);

  wind = std::make_unique<Wind>(wind_force, log_wind);

}

void Application::update() {

  // TODO: Resolve the collision between the ball and the ground in a better way

  // Iterate until the ball hits the ground.
  if ((ball->position.z >= ground_height) && (ball->is_rolling == false)) {

    // Calculates the wind force based off whether we are using the log wind
    // model or not.
    vec3 wind_force =
        get_wind_force(wind->wind, ball->position.z, wind->log_wind);

    // The ball's effective velocity, or "air speed" vector is determined by
    // taking the difference between the instantaneous velocity vector and the
    // wind vector.
    vec3 air_speed = ball->velocity - wind_force;

    ball->current_spin_rate =
        get_spin_rate(ball->launch_spin_rate, elapsed_simulation_time);

    // The coefficients of lift and drag are determined by the ball's speed
    // and spin rate. We take the square of the velocity vector here since we
    // don't need to to get the raw speed, which would involve an expensive
    // sqrt function.
    float air_speed_squared = air_speed.dot(air_speed);
    std::pair<float, float> coefficients = get_drag_and_lift_coefficients(
        air_speed_squared, ball->current_spin_rate);

    float drag_coefficient = coefficients.first;
    float lift_coefficient = coefficients.second;

    vec3 lift =
        get_lift_force(air_speed, ball->rotation_axis, lift_coefficient);
    vec3 drag = get_drag_force(air_speed, drag_coefficient);

    ball->sum_forces = lift + drag + BALL_WEIGHT;

    ball->integrate(INV_BALL_MASS, seconds_per_frame);

    if ((ball->velocity.z < 0.0f) && (ball->max_height_set == false)) {
      ball->max_height = ball->position.z;
      ball->max_height_set = true;
    }

    elapsed_simulation_time += seconds_per_frame;
  }

  // We will now resolve the collision between the ball and the ground
  if (ball->position.z <= ground_height) {

    ball->position.z = ground_height;

    // End the bounce subroutine and start the roll subroutine if the max
    // height from the previous flight part was less than the specified
    // minimum bounce height of 5 mm.
    if (ball->max_height < MIN_BOUNCE_HEIGHT) {

      ball->is_rolling = true;
      ball->acceleration.zero();
      ball->position.z = 0.0f;
      ball->velocity.z = 0.0f;

      // TODO: Compute the force of gravity tangential and normal to the local
      // terrain surface.
      // Calculate the friction on the ball from the surface of the green.
      float velocity_squared = ball->velocity.dot(ball->velocity);
      vec3 friction = get_friction_force(ball->velocity);
      ball->sum_forces = friction;


      if (velocity_squared > 0.1f) {

        ball->integrate(INV_BALL_MASS, seconds_per_frame);
        velocity_squared = ball->velocity.dot(ball->velocity);

      } else {

        ball->velocity.zero();
        ball->acceleration.zero();

        std::cout << "Rest coordinates: ";
        ball->position.display();
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

      float normal_force = std::abs(velocity_ground_y);

      // Calculate the angular velocity of the ball with respect to the ground.
      // TODO: Make this into a struct and benchmark whether to pass around a
      // pointer or not.
      ball->current_spin_rate =
          get_spin_rate(ball->launch_spin_rate, elapsed_simulation_time);
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
        collision as if the ball were colliding with a plane angled at an angle
        theta_c above the angle of the surface the ball is colliding against.
      */

      float ball_speed = norm(ball->velocity);

      float theta_c =
          GROUND_FIRMNESS * ball_speed
          * fast_atan(std::abs(velocity_ground_x / velocity_ground_y));

      // Use theta c to transform to the ball velocity vector components from
      // the x-y frame to the x'-y' frame, where the x' axis is equal to a
      // surface inclined at angle theta_c from the original surface.

      float velocity_ground_x_transformed =
          velocity_ground_x * cosf(theta_c) - normal_force * sinf(theta_c);
      float velocity_ground_y_transformed =
          velocity_ground_x * sinf(theta_c) + normal_force * cosf(theta_c);

      float normal_force_transformed = std::abs(velocity_ground_y_transformed);

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

        velocity_ground_y_transformed = restitution * normal_force_transformed;

        angular_velocity_ground_z -=
            ((5.0f * FRICTION) / (2.0f * RADIUS))
            * (normal_force_transformed * (1.0f + restitution));

      } else {

        // Linear and angular velocity in the x'-y' plane after rolling
        velocity_ground_x_transformed =
            (1.0f / 7.0f)
            * (5.0f * velocity_ground_x_transformed
               - (2.0f * RADIUS * angular_velocity_ground_z));
        velocity_ground_y_transformed = restitution * normal_force_transformed;

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
        velocity_ground_z = (-2.0f / 7.0f) * RADIUS * angular_velocity_ground_x;

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

void Application::render() {

  SDL_SetRenderDrawColor(renderer, 5, 98, 99, 255);
  SDL_RenderClear(renderer);

  Uint32 ground_color = 0xFF3b4f07;

  Sint16 groundL_x1 = 0;
  Sint16 groundL_x2 = static_cast<Sint16>(window_width / 2.0f);
  Sint16 groundL_y1 = static_cast<Sint16>(window_height);
  Sint16 groundL_y2 = static_cast<Sint16>(window_height - window_height / 3);

  boxColor(renderer, groundL_x1, groundL_y1, groundL_x2, groundL_y2,
           ground_color);

  Sint16 ground2_x1 = static_cast<Sint16>(window_width / 2.0f);
  Sint16 ground2_x2 = static_cast<Sint16>(window_width);
  Sint16 ground2_y1 = 0;
  Sint16 ground2_y2 = static_cast<Sint16>(window_height);

  boxColor(renderer, ground2_x1, ground2_y1, ground2_x2, ground2_y2,
           ground_color);

  Uint32 ball_color = 0xFFFFFFFF;
  Sint16 ball_radius = 4;

  Sint16 ballh_window_x = static_cast<Sint16>(
      ((ball->position.x + 20.0f) * PIXELS_PER_METER) / 2.0f);
  Sint16 ballh_window_y = static_cast<Sint16>(
      (static_cast<float>((window_height)
                          - (ball->position.z * PIXELS_PER_METER) / 2.0f))
      - (static_cast<float>(window_height) / 3.0f));

  filledCircleColor(renderer, ballh_window_x, ballh_window_y, ball_radius,
                    ball_color);

  Sint16 ballv_window_size = static_cast<Sint16>(window_width / 2);
  Sint16 ballv_window_x = static_cast<Sint16>(
      ballv_window_size + static_cast<float>(ballv_window_size / 2)
      - (ball->position.y * PIXELS_PER_METER));
  Sint16 ballv_window_y = static_cast<Sint16>(
      window_height - (ball->position.x * PIXELS_PER_METER * (9.0f / 16.0f)) - 100.0f);

  filledCircleColor(renderer, ballv_window_x, ballv_window_y, ball_radius,
                    ball_color);

  SDL_RenderPresent(renderer);
}

void Application::run() {

  setup();

  while (is_running) {

    auto frame_start = std::chrono::high_resolution_clock::now();

    process_input();
    update();
    render();

    auto frame_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> seconds_elapsed_for_frame =
        frame_end - frame_start;

    if (seconds_elapsed_for_frame.count() < seconds_per_frame) {

      uint32_t ms_to_wait = static_cast<uint32_t>(
          (seconds_per_frame - seconds_elapsed_for_frame.count()) * 1000.0f);

      SDL_Delay(ms_to_wait);

    }

  }

}

void Application::destroy() {

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

}