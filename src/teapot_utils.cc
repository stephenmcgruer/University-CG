//! \author Stephen McGruer

#include "./teapot_utils.h"

const float kPi = atan(1) * 4;

namespace computer_graphics {
void CreateXRotMatrix(FloatMatrix &f, float theta) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  float cosTheta = cos(theta * kPi / 180);
  float sinTheta = sin(theta * kPi / 180);

  f(0, 0) = 1.0f;
  f(0, 1) = 0.0f;
  f(0, 2) = 0.0f;
  f(0, 3) = 0.0f;

  f(1, 0) = 0.0f;
  f(1, 1) = cosTheta;
  f(1, 2) = -sinTheta;
  f(1, 3) = 0.0f;


  f(2, 0) = 0.0f;
  f(2, 1) = sinTheta;
  f(2, 2) = cosTheta;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateYRotMatrix(FloatMatrix &f, float theta) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  float cosTheta = cos(theta * kPi / 180);
  float sinTheta = sin(theta * kPi / 180);

  f(0, 0) = cosTheta;
  f(0, 1) = 0.0f;
  f(0, 2) = sinTheta;
  f(0, 3) = 0.0f;

  f(1, 0) = 0.0f;
  f(1, 1) = 1.0f;
  f(1, 2) = 0.0f;
  f(1, 3) = 0.0f;

  f(2, 0) = -sinTheta;
  f(2, 1) = 0.0f;
  f(2, 2) = cosTheta;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateZRotMatrix(FloatMatrix &f, float theta) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  float cosTheta = cos(theta * kPi / 180);
  float sinTheta = sin(theta * kPi / 180);

  f(0, 0) = cosTheta;
  f(0, 1) = -sinTheta;
  f(0, 2) = 0.0f;
  f(0, 3) = 0.0f;

  f(1, 0) = sinTheta;
  f(1, 1) = cosTheta;
  f(1, 2) = 0.0f;
  f(1, 3) = 0.0f;

  f(2, 0) = 0.0f;
  f(2, 1) = 0.0f;
  f(2, 2) = 1.0f;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateMovMatrix(FloatMatrix &f, float dx, float dy, float dz) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  f(0, 0) = 1.0f;
  f(0, 1) = 0.0f;
  f(0, 2) = 0.0f;
  f(0, 3) = dx;

  f(1, 0) = 0.0f;
  f(1, 1) = 1.0f;
  f(1, 2) = 0.0f;
  f(1, 3) = dy;

  f(2, 0) = 0.0f;
  f(2, 1) = 0.0f;
  f(2, 2) = 1.0f;
  f(2, 3) = dz;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateScaleMatrix(FloatMatrix &f, float dx, float dy, float dz) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  f(0, 0) = dx;
  f(0, 1) = 0.0f;
  f(0, 2) = 0.0f;
  f(0, 3) = 0.0f;

  f(1, 0) = 0.0f;
  f(1, 1) = dy;
  f(1, 2) = 0.0f;
  f(1, 3) = 0.0f;

  f(2, 0) = 0.0f;
  f(2, 1) = 0.0f;
  f(2, 2) = dz;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateXShearMatrix(FloatMatrix &f, float dx) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  f(0, 0) = 1.0f;
  f(0, 1) = dx;
  f(0, 2) = 0.0f;
  f(0, 3) = 0.0f;

  f(1, 0) = 0.0f;
  f(1, 1) = 1.0f;
  f(1, 2) = 0.0f;
  f(1, 3) = 0.0f;

  f(2, 0) = 0.0f;
  f(2, 1) = 0.0f;
  f(2, 2) = 1.0f;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

void CreateYShearMatrix(FloatMatrix &f, float dy) {
  if (f.num_cols() != 4 || f.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return;
  }

  f(0, 0) = 1.0f;
  f(0, 1) = 0.0f;
  f(0, 2) = 0.0f;
  f(0, 3) = 0.0f;

  f(1, 0) = dy;
  f(1, 1) = 1.0f;
  f(1, 2) = 0.0f;
  f(1, 3) = 0.0f;

  f(2, 0) = 0.0f;
  f(2, 1) = 0.0f;
  f(2, 2) = 1.0f;
  f(2, 3) = 0.0f;

  f(3, 0) = 0.0f;
  f(3, 1) = 0.0f;
  f(3, 2) = 0.0f;
  f(3, 3) = 1.0f;
}

}  // namespace computer_graphics
