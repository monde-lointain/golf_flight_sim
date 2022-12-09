#pragma once

#include "vecn.h"

struct MatMN {
  int M;      // rows
  int N;      // cols
  vecn *rows; // the rows of the matrix with N columns inside

  MatMN();
  MatMN(int M, int N);
  MatMN(const MatMN &m);
  ~MatMN();

  void zero();
  MatMN transpose() const;

  const MatMN &operator=(const MatMN &m);
  vecn operator*(const vecn &v) const;
  MatMN operator*(const MatMN &m) const;
};