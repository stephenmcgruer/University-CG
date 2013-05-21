//! \author Stephen McGruer

//A set of graphics-related functions.

#ifndef SRC_TEAPOTUTILS_H_
#define SRC_TEAPOTUTILS_H_

#include <cmath>
#include <cstdio>

#include "./float_matrix.h"

namespace computer_graphics {
//! \struct WindowInfo
//! \brief Stores information about the OpenGl window.
struct WindowInfo {
    int left;
    int right;
    int top;
    int bottom;

    WindowInfo(int wleft, int wright, int wtop, int wbottom) {
      left = wleft;
      right = wright;
      top = wtop;
      bottom = wbottom;
    }
};

// Matrix creation

//! \brief Creates a matrix to rotate an object in the x-axis around
//! the origin.
void CreateXRotMatrix(FloatMatrix &f, float theta);

//! \brief Creates a matrix to rotate an object in the y-axis around
//! the origin.
void CreateYRotMatrix(FloatMatrix &f, float theta);

//! \brief Creates a matrix to rotate an object in the z-axis around
//! the origin.
void CreateZRotMatrix(FloatMatrix &f, float theta);

//! \brief Creates a matrix to move an object in 3D space.
void CreateMovMatrix(FloatMatrix &f, float dx, float dy, float dz);

//! \brief Creates a matrix to scale an object in 3D space.
void CreateScaleMatrix(FloatMatrix &f, float dx, float dy, float dz);

//! \brief Creates a matrix to shear an object in the x-axis.
void CreateXShearMatrix(FloatMatrix &f, float dx);

//! \brief Creates a matrix to shear an object in the x-axis.
void CreateYShearMatrix(FloatMatrix &f, float dy);
}  // namespace computer_graphics

#endif  // SRC_TEAPOTUTILS_H_
