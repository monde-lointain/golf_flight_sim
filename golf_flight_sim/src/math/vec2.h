#pragma once

struct vec2 {

  float x, y;

  vec2();
  vec2(float x, float y);

  ~vec2() = default;

  void zero();
  vec2 perp() const;
  
  vec2 operator+(vec2 v) const;
  vec2 operator-(vec2 v) const;
  vec2 operator*(float n) const;
  vec2 operator/(float n) const;

  vec2 &operator+=(vec2 v);
  vec2 &operator-=(vec2 v);
  vec2 &operator*=(float n);
  vec2 &operator/=(float n);

};