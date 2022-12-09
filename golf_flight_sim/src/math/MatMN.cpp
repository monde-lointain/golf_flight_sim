#include "MatMN.h"

MatMN::MatMN() : M(0), N(0), rows(nullptr) {}

MatMN::MatMN(int M, int N) : M(M), N(N) {
  rows = new vecn[M];
  for (int i = 0; i < M; i++)
    rows[i] = vecn(N);
}

MatMN::MatMN(const MatMN &m) {
  *this = m;
}

MatMN::~MatMN() {
  delete[] rows;
}

void MatMN::zero() {
  for (int i = 0; i < M; i++)
    rows[i].zero();
}

MatMN MatMN::transpose() const {
  MatMN result(N, M);
  for (int i = 0; i < M; i++)
    for (int j = 0; j < N; j++)
      result.rows[j][i] = rows[i][j];
  return result;
}

const MatMN &MatMN::operator=(const MatMN &m) {
  delete[] rows;
  M = m.M;
  N = m.N;
  rows = new vecn[M];
  for (int i = 0; i < M; i++)
    rows[i] = m.rows[i];
  return *this;
}

vecn MatMN::operator*(const vecn &v) const {
  // Return the vector if the dimensions don't match
  if (v.N != N)
    return v;

  vecn result(M);
  for (int i = 0; i < M; i++)
    result[i] = v.dot(rows[i]);
  return result;
}

MatMN MatMN::operator*(const MatMN &m) const {
  // Return the second matrix if the dimensions don't match
  if (m.M != N && m.N != M)
    return m;

  MatMN transposed = m.transpose();
  MatMN result(M, m.N);
  for (int i = 0; i < M; i++)
    for (int j = 0; j < m.N; j++)
      result.rows[i][j] = rows[i].dot(transposed.rows[j]);
  return result;
}