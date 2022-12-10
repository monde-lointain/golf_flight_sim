#include "Ball.h"

Ball::Ball(vec3 ball_position, vec3 ball_velocity, float spin) {
  this->position = ball_position;
  this->velocity = ball_velocity;
  this->spin_rate = spin;
  this->height.push_back(position.z);
  this->max_height = position.z;
  // this->inv_mass = INV_BALL_MASS;
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