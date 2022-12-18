#pragma once

#include "../math/vec3.h"

struct Ball {

  vec3 position;
  vec3 velocity;
  vec3 acceleration;
  vec3 rotation_axis;

  vec3 wind_force;
  vec3 lift_force;
  vec3 drag_force;

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