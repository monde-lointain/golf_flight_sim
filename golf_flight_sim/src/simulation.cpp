#include "simulation.h"
#include "./Physics/Ball.h"
#include "./Physics/coefficients.h"
#include "./Physics/constants.h"
#include "./Physics/force.h"
#include "./math/trig.h"
#include "./math/unit_conversion.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

float get_spin_rate(float spin_rate, float time) {
  return spin_rate * std::expf(-time / SPIN_DECAY_RATE);
}

void run_simulation() {

  // Initialize the launch parameters
  //   float launch_speed_mph = 160.0;
  float launch_angle_deg = 10.0;
  float launch_heading_deg = 1.1f;
  float launch_spin_rate = 2000.0;
  float spin_axis_deg = 7.0;
  //   float wind_speed_mph = 0.0;
  float wind_heading_deg = 75.0;
  // Set to false to use the uniform wind profile instead of logarithmic
  bool log_wind = true;

  // Convert the units of the launch parameters
  //   float launch_speed = mph_to_ms(launch_speed_mph);
  float launch_speed = 75.0;
  float launch_angle = deg_to_rad(launch_angle_deg);
  float launch_heading = deg_to_rad(launch_heading_deg);
  float spin_axis_2d = deg_to_rad(spin_axis_deg);
  //   float wind_speed = mph_to_ms(wind_speed_mph);
  float wind_speed = 15.0;
  float wind_heading = deg_to_rad(wind_heading_deg);

  // Set the initial position and velocity vectors
  auto ball_position = vec3(0.0, 0.0, 0.0);
  auto ball_velocity =
      vec3(launch_speed * cosf(launch_angle) * cosf(launch_heading),
           launch_speed * cosf(launch_angle) * sinf(launch_heading),
           launch_speed * sinf(launch_angle));

  // Calculate the 3D axis of rotation based on the launch heading and the spin
  // axis angle.
  auto rotation_axis =
      vec3(cosf(spin_axis_2d) * sinf(launch_heading),
           -cosf(spin_axis_2d) * cosf(launch_heading), 
           spin_axis_2d);

  // Create the ball struct and initialize it with it's starting position and
  // velocity.

  // TODO: Benchmark this to see what would happen if you just passed around the
  // raw struct.
  //std::unique_ptr<Ball> ball =
  //    std::make_unique<Ball>(ball_position, ball_velocity, launch_spin_rate);

  // This is WAY faster than using a pointer (~35 microsec vs ~41) but I'm not
  // sure if we're able to pass it around to the renderer like this. Need to
  // figure that out.
  Ball ball = Ball(ball_position, ball_velocity, launch_spin_rate);

  float ground_height = ball.position.z;

  // std::cout << "Initial velocity: ";
  // ball_velocity.display();

  float elapsed_time = 0.0;
  float dt = 0.01f;

  uint64_t i = 0;

  // Start the shot calculations
  while (true) {

    /*
      Flight subroutine
    */ 

    // Iterate until the ball hits the ground.
    while (ball.position.z >= ground_height) {

      elapsed_time = static_cast<float>(i) * dt;

      vec3 wind_force =
          get_wind_force(wind_speed, wind_heading, ball.position.z, log_wind);

      // The ball's effective velocity, or "air speed" vector is determined by
      // taking the difference between the instantaneous velocity vector and the
      // wind vector.
      vec3 air_speed = ball.velocity - wind_force;

      ball.spin_rate = get_spin_rate(launch_spin_rate, elapsed_time);

      // The coefficients of lift and drag are determined by the ball's speed
      // and spin rate. We take the square of the velocity vector here since we
      // don't need to to get the raw speed, which would involve an expensive
      // sqrt function.
      float air_speed_squared = air_speed.dot(air_speed);
      std::pair<float, float> coefficients =
          get_drag_and_lift_coefficients(air_speed_squared, ball.spin_rate);

      float drag_coefficient = coefficients.first;
      float lift_coefficient = coefficients.second;

      vec3 lift = get_lift_force(air_speed, rotation_axis, lift_coefficient);
      vec3 drag = get_drag_force(air_speed, drag_coefficient);

      ball.sum_forces = lift + drag + BALL_WEIGHT;

      ball.integrate(INV_BALL_MASS, dt);

      if ((ball.velocity.z < 0.0f) && (ball.max_height_set == false)) {
        ball.max_height = ball.position.z;
        ball.max_height_set = true;
      }

      i += 1;

    }

    /*
      Bounce subroutine
    */ 

    // We will now resolve the collision between the ball and the ground
    if (ball.position.z <= ground_height) {

      ball.position.z = ground_height;

      elapsed_time = static_cast<float>(i) * dt;

      // End the bounce subroutine and start the roll subroutine if the max
      // height from the previous flight part was less than the specified
      // minimum bounce height of 5 mm.
      if (ball.max_height < MIN_BOUNCE_HEIGHT) {
        break;
      }

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
      auto z_unit = vec3(ball.velocity.cross(y_unit));
      z_unit /= norm(z_unit);
      auto x_unit = vec3(y_unit.cross(z_unit));

      // Calculate the new 2D velocity vector with respect to the local ground
      // frame
      float velocity_ground_x = ball.velocity.dot(x_unit);
      float velocity_ground_y = ball.velocity.dot(y_unit);

      // Gross but it works. TODO: Learn the minutae of floating point comparisons.
      assert(static_cast<int>(ball.velocity.dot(z_unit)) == 0);

      float normal_force = std::abs(velocity_ground_y);

      // Calculate the angular velocity of the ball with respect to the ground.
      // TODO: Make this into a struct and benchmark whether to pass around a
      // pointer or not.
      ball.spin_rate = get_spin_rate(launch_spin_rate, elapsed_time);
      float angular_velocity_ground_x =
          rpm_to_rad_s(ball.spin_rate) * rotation_axis.dot(x_unit);
      float angular_velocity_ground_y =
          rpm_to_rad_s(ball.spin_rate) * rotation_axis.dot(y_unit);
      float angular_velocity_ground_z =
          rpm_to_rad_s(ball.spin_rate) * rotation_axis.dot(z_unit);

      /*
        When the ball hits the ground, it tends to penetrate into the ground
        and slip across it. These forces act as both a linear and angular
        impulse on the ball, changing its linear and angular velocity
        differently than how one would expect from a normal inelastic
        collision. We can represent these interactions by thinking of the
        collision as if the ball were colliding with a plane angled at an angle
        theta_c above the angle of the surface the ball is colliding against.
      */

      float ball_speed = norm(ball.velocity);

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
      ball.velocity = (velocity_ground_x * x_unit)
                       + (velocity_ground_y * y_unit)
                       + (velocity_ground_z * z_unit);

      vec3 angular_velocity = (angular_velocity_ground_x * x_unit)
                              + (angular_velocity_ground_y * y_unit)
                              + (angular_velocity_ground_z * z_unit);

      launch_spin_rate = rad_s_to_rpm(norm(angular_velocity));

      rotation_axis = angular_velocity / norm(angular_velocity);

      ball.max_height_set = false;

    }

  }

  /*
    Roll subroutine
  */ 

  ball.acceleration.zero();
  ball.position.z = 0.0f;
  ball.velocity.z = 0.0f; 

  // TODO: Add acceleration in here as well. Need to keep the ball rolling if it
  // goes up a hill, stops and then comes down again.
  while (norm(ball.velocity) > 0.01) {

    // TODO: Compute the force of gravity tangential and normal to the local
    // terrain surface.
    // Calculate the friction on the ball from the surface of the green.
    vec3 friction = get_friction_force(ball.velocity);

    ball.sum_forces = friction;

    ball.integrate(INV_BALL_MASS, dt);

  }

  //std::cout << "Rest coordinates: ";
  //ball.position.display();

}