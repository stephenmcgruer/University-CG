//! \author Stephen McGruer

#include "./vertex.h"

namespace computer_graphics {

Vertex& Vertex::operator=(Vertex &other) {
  coordinates_[0] = other[0];
  coordinates_[1] = other[1];
  coordinates_[2] = other[2];

  return *this;
}

Vertex& Vertex::operator+=(Vertex &other) {
  coordinates_[0] += other[0];
  coordinates_[1] += other[1];
  coordinates_[2] += other[2];

  return *this;
}

float& Vertex::operator[] (int index) {
  return coordinates_[index];
}

}  // namespace computer_graphics
