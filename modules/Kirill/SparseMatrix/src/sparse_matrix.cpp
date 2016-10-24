#include "sparse_matrix.h"

SpareMatrix::SpareMatrix(TYPE* value, int* col, int* rowIndex, const size_t nz, const size_t nRows) {
  nz_ = nz;
  nRows_ = nRows;
  value_ = new TYPE[nz_];
  col_ = new int[nz_];
  rowIndex_ = new int[nRows_ + 1];
  std::copy(value, value + nz, value_);
  std::copy(col, col + nz, col_);
  std::copy(rowIndex, rowIndex + nRows + 1, rowIndex_);
}

SpareMatrix::~SpareMatrix() {
  delete[] value_;
  delete[] col_;
  delete[] rowIndex_;
}

vector SpareMatrix::operator*(const vector& v) {
  vector result(nRows_);
  TYPE localSum;
  #pragma omp parallel private(localSum) num_threads(2) if (ENABLE_PARALLEL)
  {
    #pragma omp for nowait
    for (int i = 0; i < nRows_; i++) {
      localSum = 0;
      for (int j = rowIndex_[i]; j < rowIndex_[i + 1]; j++)
        localSum += value_[j] * v[col_[j]];
      result[i] = localSum;

    }
  }
  return result;
}