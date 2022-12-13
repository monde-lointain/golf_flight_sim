#include "Ball.h"
#include <iostream>

Ball::Ball(vec3 ball_position, vec3 ball_velocity, vec3 rotation_axis,
           float spin) {

  this->position = ball_position;
  this->velocity = ball_velocity;
  this->acceleration = vec3(0.0, 0.0, 0.0);
  this->rotation_axis = rotation_axis;

  this->current_spin_rate = spin;
  this->launch_spin_rate = spin;
  this->elapsed_time = 0.0f;

  this->max_height = position.z;
  this->max_height_set = false;

  this->is_rolling = false;

  this->sum_forces = vec3(0.0, 0.0, 0.0);

}

void Ball::clear_forces() {
  sum_forces = vec3(0.0, 0.0, 0.0);
}

void Ball::integrate(float dt) {

  acceleration = sum_forces * INV_BALL_MASS;

  // Integrate acceleration to find velocity and position
  velocity += acceleration * dt;
  position += velocity * dt;

  clear_forces();

}
