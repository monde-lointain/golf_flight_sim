#pragma once

#include "vec3.h"

//const float BALL_RADIUS = 0.021335f;
//const float BALL_REFERENCE_AREA = 0.00142999713410465632357338159458f; // in m^2
const float INV_BALL_MASS = 21.77226213803614f; // 1/mass (mass in kg)

struct Ball {

  vec3 position;
  vec3 velocity;
  vec3 acceleration;
  vec3 rotation_axis;

  float current_spin_rate;
  float launch_spin_rate;
  float max_height;
  bool max_height_set;
  bool is_rolling;

  vec3 sum_forces;
  //vec3 sum_forces{};
  //float inv_mass;

  Ball(vec3 ball_position, vec3 ball_velocity, vec3 rotation_axis, float spin);
  ~Ball() = default;

  void clear_forces();
  //void integrate(float dt);
  void integrate(float inv_mass, float dt);

};