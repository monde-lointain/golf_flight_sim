#pragma once

#include "../Components/Wind.h"
#include "../math/vec3.h"
#include <memory>

vec3 get_wind_force(std::unique_ptr<Wind> &wind, float ball_height);
vec3 get_lift_force(vec3 velocity, vec3 rotation_axis, float lift_coefficient);
vec3 get_drag_force(vec3 velocity, float drag_coefficient);
vec3 get_friction_force(vec3 velocity);