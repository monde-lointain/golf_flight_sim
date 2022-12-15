#include "force.h"
#include "constants.h"
#include "../math/unit_conversion.h"
#include <cmath>

vec3 get_wind_force(std::unique_ptr<Wind> &wind, float ball_height) {

  // The wind z-component will always be assumed to be zero. That is, the wind
  // will always be assumed to be blowing horizontally, instead of up or down
  // towards the ground.
  // We only convert the wind speed here because we need it in mph for
  // everything else (the UI stuff).
  float wind_speed_ms = mph_to_ms(wind->speed);
  auto wind_force = vec3(wind_speed_ms * cosf(wind->direction),
                         wind_speed_ms * sinf(wind->direction), 0.0);

  if (ball_height < ROUGHNESS_LENGTH_SCALE) {
    ball_height = ROUGHNESS_LENGTH_SCALE;
  }

  if (wind->log_wind) {

    // Adjust the wind force based on the height of the ball according to
    // the logarithmic wind profile.
    wind_force *=
        std::logf(ball_height / ROUGHNESS_LENGTH_SCALE)
        / std::logf(LOG_WIND_PROFILE_REFERENCE_HEIGHT / ROUGHNESS_LENGTH_SCALE);

  }

  return wind_force;

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