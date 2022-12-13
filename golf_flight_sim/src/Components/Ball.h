#pragma once

#include "../Physics/vec3.h"

const float INV_BALL_MASS = 21.77226213803614f; // 1/mass (mass in kg)

struct Ball {

  vec3 position;
  vec3 velocity;
  vec3 acceleration;
  vec3 rotation_axis;

  float current_spin_rate;
  float launch_spin_rate;
  float elapsed_time;

  float max_height;
  bool max_height_set;

  bool is_rolling;

  vec3 sum_forces;

  Ball(vec3 ball_position, vec3 ball_velocity, vec3 rotation_axis, float spin);
  ~Ball() = default;

  void clear_forces();

  void integrate(float dt);

};