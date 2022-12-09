#pragma once

struct vecn {
  int N;
  float *data;

  vecn();
  vecn(int N);
  vecn(const vecn &v);
  ~vecn();

  void zero();                             // v1.zero()
  float dot(const vecn &v) const;          // v1.dot(v2)

  vecn &operator=(const vecn &v);          // v1 = v2
  vecn operator+(const vecn &v) const;     // v1 + v2
  vecn operator-(const vecn &v) const;     // v1 - v2
  vecn operator*(const float n) const;     // v1 * n
  const vecn &operator+=(const vecn &v);   // v1 += v2
  const vecn &operator-=(const vecn &v);   // v1 -= v2
  const vecn &operator*=(const float n);   // v1 *= n
  float operator[](const int index) const; // v1[index]
  float &operator[](const int index);      // v1[index]
};