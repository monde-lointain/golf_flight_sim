#pragma once

inline float deg_to_rad(float deg) {
  return deg * 0.017453293f;
}

inline float rad_to_deg(float rad) {
  return rad * 57.295777937f;
}

inline float mph_to_ms(float mph) {
  return mph * 0.44704f;
}

inline float ms_to_mph(float ms) {
  return ms * 2.2369362920f;
}

inline float rpm_to_rad_s(float rpm) {
  // rpm * (2pi / 60)
  return rpm * 0.10471975511965977f;
}

inline float rad_s_to_rpm(float rad_s) {
  // rad/s * (60 / 2pi)
  return rad_s * 9.549296585513720146f;
}

inline float m_to_yd(float m) {
  return m * 1.09361f;
}

inline float m_to_ft(float m) {
  return m * 3.28084f;
}