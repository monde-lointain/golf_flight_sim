#pragma once

#include "../math/vec3.h"

const float PI = 3.14159265358979323846264338327950288f;
const float RADIUS = 0.0213f;
const float GRAVITY = 9.81f;
const vec3 GRAVITY_VEC(0.0f, 0.0f, -9.81f);
const vec3 BALL_WEIGHT(0.0f, 0.0f, -0.450279f);
const float INV_BALL_MASS = 21.77226213803614f; // 1/mass (mass in kg)
const float ROUGHNESS_LENGTH_SCALE = 0.4f;
const float LOG_WIND_PROFILE_REFERENCE_HEIGHT = 10.0; // in meters
const float SPIN_DECAY_RATE = 24.5;
const float MIN_BOUNCE_HEIGHT = 0.005f;
const float MIN_ROLL_VELOCITY_SQUARED = 0.0001f;
static float GROUND_FIRMNESS = 0.0186477f;
const float FRICTION = 0.4f;

// Lift and drag constants: equal to 0.5, times the reference area of the golf
// ball (0.001425 m^2), times the air density (1.2 kg/m^3)
const float LIFT_CONST = 0.0008551855026042919f;

// The drag coefficient is negative because drag acts in the opposite direction
// of the velocity vector.
const float DRAG_CONST = -0.0008551855026042919f;

/*
  The formula for the coefficient of friction for rolling on a green is:
        u_r = 0.784 / dist,
  where dist is the stimpmeter distance in feet.

  For this simulation we're assuming a stimpmeter of 6.
*/
static float FRICTION_ROLL = 0.131f;
