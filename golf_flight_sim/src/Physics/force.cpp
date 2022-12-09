#include "force.h"
#include "constants.h"
#include <cmath>

vec3 get_wind_force(float wind_speed, float wind_heading, float ball_height,
                    bool log_wind) {

  // The wind z-component will always be assumed to be zero. That is, the wind
  // will always be assumed to be blowing horizontally, instead of up or down
  // towards the ground.
  auto wind = vec3(wind_speed * cosf(wind_heading),
                   wind_speed * sinf(wind_heading), 0.0);

  if (ball_height < ROUGHNESS_LENGTH_SCALE) {
    ball_height = ROUGHNESS_LENGTH_SCALE;
  }

  if (log_wind) {
    // Adjust the wind force based on the height of the ball according to the
    // logarithmic wind profile.
    wind *=
        std::logf(ball_height / ROUGHNESS_LENGTH_SCALE)
        / std::logf(LOG_WIND_PROFILE_REFERENCE_HEIGHT / ROUGHNESS_LENGTH_SCALE);
  }

  return wind;

}

vec3 get_lift_force(vec3 velocity, vec3 rotation_axis, float lift_coefficient) {

  vec3 lift_force = LIFT_CONST * lift_coefficient
                    * norm(rotation_axis.cross(velocity))
                    * rotation_axis.cross(velocity);

  return lift_force;

}

vec3 get_drag_force(vec3 velocity, float drag_coefficient) {

  vec3 drag_force = DRAG_CONST * drag_coefficient * norm(velocity) * velocity;

  return drag_force;

}

vec3 get_friction_force(vec3 velocity) {

    vec3 friction_direction = -velocity.unit_vector();

    float friction_magnitude = (5.0f / 7.0f) * FRICTION_ROLL * norm(BALL_WEIGHT);

    vec3 friction = friction_direction * friction_magnitude;

    return friction;

}