#include "vec2.h"

vec2::vec2() : x(0.0f), y(0.0f) {}

vec2::vec2(float x, float y) : x(x), y(y) {}

void vec2::zero() {
  this->x = 0.0f;
  this->y = 0.0f;
}

vec2 vec2::perp() const {
  vec2 result;
  result.x = this->y;
  result.y = -this->x;
  return result;
}

vec2 vec2::operator+(vec2 v) const {
  vec2 result;
  result.x = this->x + v.x;
  result.y = this->y + v.y;
  return result;
}

vec2 vec2::operator-(vec2 v) const {
  vec2 result;
  result.x = this->x - v.x;
  result.y = this->y - v.y;
  return result;
}

vec2 vec2::operator*(float n) const {
  vec2 result;
  result.x = this->x * n;
  result.y = this->y * n;
  return result;
}

vec2 vec2::operator/(float n) const {
  vec2 result;
  result.x = this->x / n;
  result.y = this->y / n;
  return result;
}

vec2 &vec2::operator+=(vec2 v) {
  this->x = this->x + v.x;
  this->y = this->y + v.y;
  return *this;
}

vec2 &vec2::operator-=(vec2 v) {
  this->x = this->x - v.x;
  this->y = this->y - v.y;
  return *this;
}

vec2 &vec2::operator*=(float n) {
  this->x = this->x * n;
  this->y = this->y * n;
  return *this;
}

vec2 &vec2::operator/=(float n) {
  this->x = this->x / n;
  this->y = this->y / n;
  return *this;
}