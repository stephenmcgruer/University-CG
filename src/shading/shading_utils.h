//! \author Stephen McGruer

//A set of shading-related functions.

#ifndef SRC_SHADING_SHADINGUTILS_H_
#define SRC_SHADING_SHADINGUTILS_H_

#include <opencv/highgui.h>

#include "../vertex.h"

namespace computer_graphics {
//! Projects a vector to be seen from view_position.
void Project(Vertex& vector, Vertex view_position, int width, int height);

//! Normalises a vertex, making it's magnitude one.
void Normalise(Vertex& v);

//! Computes the dot product of two (3D) vertices.
float DotProduct(Vertex a, Vertex b);

//! \brief Computes the surface normal of a triangle, and places it in the
//!        normal variable.
//!
//! Assumes that p1, p2, p3 are given in clockwise order.
void ComputeSurfaceNormal(Vertex p1, Vertex p2, Vertex p3, Vertex& normal);

//! Checks if a point (x,y) is in the triangle given by p1, p2, and p3.
bool InTriangle(int x, int y, Vertex p1, Vertex p2, Vertex p3);

//! \brief Calculates the barycentric coordinates for a point (x,y) in the
//! triangle p1, p2, p3.
void CalculateBarycentricCoordinates(int, int, Vertex, Vertex, Vertex,
    float& alpha, float& beta, float& gamma);
}  // namespace computer_graphics

#endif  // SRC_SHADING_SHADINGUTILS_H_
