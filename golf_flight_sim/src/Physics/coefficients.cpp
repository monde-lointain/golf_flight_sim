#include "coefficients.h"

const float DRAG_AND_LIFT_COEFFICIENTS_ARR[10][7][2] =
    {{{0.52f, -0.11f}, {0.39f, -0.06f}, {0.36f, 0.06f}, {0.42f, 0.35f}, {0.40f, 0.39f}, {0.48f, 0.41f}, {0.52f, 0.49f}},
     {{0.33f,  0.00f}, {0.25f,  0.12f}, {0.28f, 0.18f}, {0.36f, 0.33f}, {0.38f, 0.36f}, {0.43f, 0.38f}, {0.45f, 0.45f}},
     {{0.22f,  0.06f}, {0.24f,  0.17f}, {0.27f, 0.24f}, {0.31f, 0.29f}, {0.34f, 0.33f}, {0.37f, 0.34f}, {0.39f, 0.39f}},
     {{0.23f,  0.07f}, {0.23f,  0.14f}, {0.25f, 0.19f}, {0.28f, 0.24f}, {0.30f, 0.28f}, {0.33f, 0.31f}, {0.36f, 0.35f}},
     {{0.24f,  0.07f}, {0.24f,  0.13f}, {0.25f, 0.16f}, {0.27f, 0.20f}, {0.28f, 0.24f}, {0.30f, 0.27f}, {0.34f, 0.31f}},
     {{0.24f,  0.07f}, {0.24f,  0.12f}, {0.25f, 0.15f}, {0.26f, 0.18f}, {0.26f, 0.21f}, {0.29f, 0.24f}, {0.32f, 0.28f}},
     {{0.25f,  0.08f}, {0.25f,  0.12f}, {0.25f, 0.14f}, {0.26f, 0.17f}, {0.26f, 0.19f}, {0.28f, 0.22f}, {0.29f, 0.26f}},
     {{0.25f,  0.08f}, {0.25f,  0.12f}, {0.25f, 0.14f}, {0.26f, 0.16f}, {0.26f, 0.18f}, {0.28f, 0.20f}, {0.29f, 0.23f}},
     {{0.25f,  0.07f}, {0.25f,  0.11f}, {0.25f, 0.13f}, {0.26f, 0.15f}, {0.26f, 0.17f}, {0.27f, 0.18f}, {0.28f, 0.22f}},
     {{0.24f,  0.07f}, {0.24f,  0.11f}, {0.25f, 0.13f}, {0.26f, 0.15f}, {0.26f, 0.16f}, {0.27f, 0.17f}, {0.27f, 0.20f}}};


std::pair<float, float> get_drag_and_lift_coefficients(float air_speed_squared,
                                                       float spin_rate) {

  // Indexes through the lift and drag coefficients array based off the air
  // speed and spin rate. We use the square of the air speed so we don't have
  // to take the square root of the air speed vec to find the magnitude.
  int row;
  int col;

  if (air_speed_squared > 7249.0f) {

    row = 9;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 5939.0f) {

    row = 8;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 4698.0f) {

    row = 7;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 3588.0f) {

    row = 6;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 2654.0f) {

    row = 5;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 1874.0f) {

    row = 4;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 1226.0f) {

    row = 3;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 705.0f) {

    row = 2;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else if (air_speed_squared > 338.0f) {

    row = 1;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  } else {

    row = 0;

    if (spin_rate > 5478.0f) {
      col = 6;
    } else if (spin_rate > 4223.0f) {
      col = 5;
    } else if (spin_rate > 3283.0f) {
      col = 4;
    } else if (spin_rate > 2340.0f) {
      col = 3;
    } else if (spin_rate > 1433.0f) {
      col = 2;
    } else if (spin_rate > 500.0f) {
      col = 1;
    } else {
      col = 0;
    }

  }

  float drag_coefficient = DRAG_AND_LIFT_COEFFICIENTS_ARR[row][col][0];
  float lift_coefficient = DRAG_AND_LIFT_COEFFICIENTS_ARR[row][col][1];

  return std::make_pair(drag_coefficient, lift_coefficient);

}

float get_coefficient_of_restitution(float velocity_along_normal) {

  float restitution = 0.12f;

  if (velocity_along_normal <= 20.0f) {
    restitution =
        0.51f - (0.0375f * velocity_along_normal)
        + (0.000903f * (velocity_along_normal * velocity_along_normal));
  }

  return restitution;

}
