#pragma once

#include <utility>

std::pair<float, float> get_drag_and_lift_coefficients(float air_speed_squared,
                                                       float spin_rate);
float get_coefficient_of_restitution(float velocity_along_normal);