#include "vecn.h"

vecn::vecn() : N(0), data(nullptr) {}

vecn::vecn(int N) : N(N) {
  data = new float[N];
}

vecn::vecn(const vecn &v) {
  N = v.N;
  data = new float[N];
  for (int i = 0; i < N; i++)
    data[i] = v.data[i];
}

vecn::~vecn() {
  delete[] data;
}

void vecn::zero() {
  for (int i = 0; i < N; i++)
    data[i] = 0.0f;
}

float vecn::dot(const vecn &v) const {
  float sum = 0.0f;
  for (int i = 0; i < N; i++)
    sum += data[i] * v.data[i];
  return sum;
}

vecn &vecn::operator=(const vecn &v) {
  delete[] data;
  N = v.N;
  data = new float[N];
  for (int i = 0; i < N; i++)
    data[i] = v.data[i];
  return *this;
}

vecn vecn::operator*(float n) const {
  vecn result = *this;
  result *= n;
  return result;
}

vecn vecn::operator+(const vecn &v) const {
  vecn result = *this;
  for (int i = 0; i < N; i++)
    result.data[i] += v.data[i];
  return result;
}

vecn vecn::operator-(const vecn &v) const {
  vecn result = *this;
  for (int i = 0; i < N; i++)
    result.data[i] -= v.data[i];
  return result;
}

const vecn &vecn::operator*=(float n) {
  for (int i = 0; i < N; i++)
    data[i] *= n;
  return *this;
}

const vecn &vecn::operator+=(const vecn &v) {
  for (int i = 0; i < N; i++)
    data[i] += v.data[i];
  return *this;
}

const vecn &vecn::operator-=(const vecn &v) {
  for (int i = 0; i < N; i++)
    data[i] -= v.data[i];
  return *this;
}

float vecn::operator[](const int index) const {
  return data[index];
}

float &vecn::operator[](const int index) {
  return data[index];
}
