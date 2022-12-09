#include "vec3.h"
#include <iostream>

vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {}

vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

void vec3::display() const {
  std::cout << "(" << x << ", " << y << ", " << z << ")"
            << "\n";
}

void vec3::zero() {
  this->x = 0.0f;
  this->y = 0.0f;
  this->z = 0.0f;
}

vec3 vec3::unit_vector() const {
  vec3 result = vec3(0.0f, 0.0f, 0.0f);
  float length = norm(*this);

  if (length != 0.0f) {
    result.x = x / length;
    result.y = y / length;
    result.z = z / length;
  }

  return result;
}

float vec3::dot(vec3 v) const {
  return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);
}

vec3 vec3::cross(vec3 v) const {
  vec3 result;
  result.x = (this->y * v.z) - (this->z * v.y);
  result.y = (this->z * v.x) - (this->x * v.z);
  result.z = (this->x * v.y) - (this->y * v.x);
  return result;
}

vec3 vec3::operator+(vec3 v) const {
  vec3 result;
  result.x = this->x + v.x;
  result.y = this->y + v.y;
  result.z = this->z + v.z;
  return result;
}

vec3 vec3::operator-(vec3 v) const {
  vec3 result;
  result.x = this->x - v.x;
  result.y = this->y - v.y;
  result.z = this->z - v.z;
  return result;
}

vec3 vec3::operator*(float n) const {
  vec3 result;
  result.x = this->x * n;
  result.y = this->y * n;
  result.z = this->z * n;
  return result;
}

vec3 vec3::operator/(float n) const {
  vec3 result;
  result.x = this->x / n;
  result.y = this->y / n;
  result.z = this->z / n;
  return result;
}

vec3 &vec3::operator+=(vec3 v) {
  this->x = this->x + v.x;
  this->y = this->y + v.y;
  this->z = this->z + v.z;
  return *this;
}

vec3 &vec3::operator-=(vec3 v) {
  this->x = this->x - v.x;
  this->y = this->y - v.y;
  this->z = this->z - v.z;
  return *this;
}

vec3 &vec3::operator*=(float n) {
  this->x = this->x * n;
  this->y = this->y * n;
  this->z = this->z * n;
  return *this;
}

vec3 &vec3::operator/=(float n) {
  this->x = this->x / n;
  this->y = this->y / n;
  this->z = this->z / n;
  return *this;
}

float norm(vec3 v) {
  return std::sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}


vec3 operator-(vec3 v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

vec3 operator*(float n, vec3 v) {
  v.x = v.x * n;
  v.y = v.y * n;
  v.z = v.z * n;
  return v;
}