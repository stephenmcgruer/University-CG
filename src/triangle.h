//! \author Stephen McGruer

#ifndef SRC_TRIANGLE_H_
#define SRC_TRIANGLE_H_

namespace computer_graphics {

//! \class Triangle
//! \brief Represents a triangle.
//!
//! The triangle vertices are stored as indices into the TriangleMesh's
//! list.
class Triangle {
  public:
    Triangle(int v1, int v2, int v3) {
      triangle_vertices_[0] = v1;
      triangle_vertices_[1] = v2;
      triangle_vertices_[2] = v3;
    }

    inline int operator[](int i) {
      return triangle_vertices_[i];
    }
  private:
    friend class TriangleMesh;

    int triangle_vertices_[3];
};

}  // namespace computer_graphics

#endif  // SRC_TRIANGLE_H_
