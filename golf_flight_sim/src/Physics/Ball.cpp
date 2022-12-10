#include "Ball.h"

Ball::Ball(vec3 ball_position, vec3 ball_velocity, float spin) {
  this->position = ball_position;
  this->velocity = ball_velocity;
  this->spin_rate = spin;
  this->max_height = position.z;
  this->max_height_set = false;
}

 void Ball::clear_forces() {
   sum_forces = vec3(0.0, 0.0, 0.0);
 }

// void Ball::integrate(float dt) {
//   acceleration = sum_forces * inv_mass;
//
//   // Integrate acceleration to find velocity and position
//   velocity += acceleration * dt;
//   position += velocity * dt;
//
//   clear_forces();
// }

void Ball::integrate(float inv_mass, float dt) {
  acceleration = sum_forces * inv_mass;

  // Integrate acceleration to find velocity and position
  velocity += acceleration * dt;
  position += velocity * dt;

  clear_forces();
}