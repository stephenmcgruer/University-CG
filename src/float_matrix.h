//! \author Stephen McGruer

#ifndef SRC_FLOATMATRIX_H_
#define SRC_FLOATMATRIX_H_

#include <cstdio>
#include <vector>

namespace computer_graphics {

//! \class FloatMatrix
//! \brief Represents an n-by-m matrix of floats.
//!
//! An n-by-m matrix of float primitives. Supports element access
//! via the () operator - e.g. f(4,2) to access the fourth row,
//! second column element. Also supports matrix multiplication,
//! but not *=.
class FloatMatrix {
  public:
    FloatMatrix(int rows, int cols);

    int size() const { return num_rows_ * num_cols_; }
    int num_rows() const { return num_rows_; }
    int num_cols() const { return num_cols_; }

    float operator() (int row, int col) const;
    float& operator() (int row, int col);

    //! Print out the matrix in a (reasonably) human-readable format.
    void PrintMatrix();

    friend FloatMatrix operator*(const FloatMatrix&, const FloatMatrix&);

  private:
    //! The matrix data is stored in a flat structure.
    std::vector<float> data_;
    int num_rows_;
    int num_cols_;
};

//! Multiplies two FloatMatrixs together.
extern FloatMatrix operator*(FloatMatrix& a, FloatMatrix& b);
}

#endif  // SRC_FLOATMATRIX_H_
