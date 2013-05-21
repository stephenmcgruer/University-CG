//! \author Stephen McGruer

#ifndef SRC_VERTEX_H_
#define SRC_VERTEX_H_

#include "triangle.h"

namespace computer_graphics {

// Forward declaration.
class TriangleMesh;

//! \class Vertex
//! \brief Represents a three dimensional vertex.
//!
//! Allows access to the x, y, and z coordinates using the [] operator.
class Vertex {
  public:
    Vertex() {
      coordinates_[0] = 0.0f;
      coordinates_[1] = 0.0f;
      coordinates_[2] = 0.0f;
    }

    Vertex(float x, float y, float z) {
      coordinates_[0] = x;
      coordinates_[1] = y;
      coordinates_[2] = z;
    }

    Vertex(float x, float y, float z, float r, float g, float b) {
      coordinates_[0] = x;
      coordinates_[1] = y;
      coordinates_[2] = z;

      red_ = r;
      green_ = g;
      blue_ = b;
    }

    Vertex & operator= (Vertex &other);
    Vertex & operator+= (Vertex &other);
    float& operator[] (int i);

    inline void set_x(float x) { coordinates_[0] = x; }
    inline void set_y(float y) { coordinates_[1] = y; }
    inline void set_z(float z) { coordinates_[2] = z; }

    inline float red() const { return red_; }
    inline float& red() { return red_; }
    inline float green() const { return green_; }
    inline float& green() { return green_; }
    inline float blue() const { return blue_; }
    inline float& blue() { return blue_; }
  private:
    float coordinates_[3];
    float red_;
    float green_;
    float blue_;
};
}  // namespace computer_graphics

#endif  // SRC_VERTEX_H_
