//! \author Stephen McGruer

#include "./float_matrix.h"

namespace computer_graphics {

FloatMatrix::FloatMatrix(int rows, int cols)
    : data_(rows * cols, 0.0f),
      num_rows_(rows),
      num_cols_(cols) {
}

float& FloatMatrix::operator()(int row, int col) {
  return data_[row * num_cols_ + col];
}

void FloatMatrix::PrintMatrix() {
  printf("FloatMatrix: %dx%d\n", num_cols_, num_rows_);
  for (int row = 0; row < num_rows_; row++) {
    printf("|");
    for (int col = 0; col < num_cols_; col++) {
      printf("%f ", operator()(row, col));
    }
    printf("|\n");
  }
  printf("\n");
}

FloatMatrix operator*(FloatMatrix& a, FloatMatrix& b) {
  if (a.num_cols() != b.num_rows()) {
    return FloatMatrix(0, 0);
  }

  FloatMatrix result(a.num_rows(), b.num_cols());
  for (int i = 0; i < result.num_rows(); i++) {
    for (int j = 0; j < result.num_cols(); j++) {
      result(i, j) = 0;
      for (int k = 0; k < a.num_cols(); k++) {
        result(i, j) += a(i, k) * b(k, j);
      }
    }
  }

  return result;
}
}
