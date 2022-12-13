#pragma once

#include <string>

struct vec3 {

  float x, y, z;

  vec3();
  vec3(float x, float y, float z);

  ~vec3() = default;

  void display() const;
  std::string to_str() const;

  void zero();
  vec3 unit_vector() const;

  float dot(vec3 v) const;
  vec3 cross(vec3 v) const;

  vec3 operator+(vec3 v) const;
  vec3 operator-(vec3 v) const;
  vec3 operator*(float n) const;
  vec3 operator/(float n) const;

  vec3 &operator+=(vec3 v);
  vec3 &operator-=(vec3 v);
  vec3 &operator*=(float n);
  vec3 &operator/=(float n);

};

float norm(vec3 v);

vec3 operator-(vec3 v);
vec3 operator*(float n, vec3 v);