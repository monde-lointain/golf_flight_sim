#pragma once

#include "vec3.h"

vec3 get_wind_force(vec3 wind, float ball_height, bool log_wind);
vec3 get_lift_force(vec3 velocity, vec3 rotation_axis, float lift_coefficient);
vec3 get_drag_force(vec3 velocity, float drag_coefficient);
vec3 get_friction_force(vec3 velocity);