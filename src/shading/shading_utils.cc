//! \author Stephen McGruer

#include "./shading_utils.h"

namespace computer_graphics {
void Project(Vertex& vector, Vertex view_position, int width, int height) {
  float dist = -2.0f;

  // Minus off the view coords
  float x = vector[0] - view_position[0];
  float y = vector[1] - view_position[1];
  float z = vector[2] - view_position[2];

  x = ((dist * x) / z) * (width / 2);
  y = ((dist * y) / z) * (height / 2);

  vector[0] = x;
  vector[1] = y;
  vector[2] = z;
}

void Normalise(Vertex &v) {
  float length = std::sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));

  v[0] /= length;
  v[1] /= length;
  v[2] /= length;
}

float DotProduct(Vertex a, Vertex b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void ComputeSurfaceNormal(Vertex p1, Vertex p2, Vertex p3, Vertex& normal) {
  float u0 = p2[0] - p1[0];
  float u1 = p2[1] - p1[1];
  float u2 = p2[2] - p1[2];
  Vertex u(u0, u1, u2);

  float v0 = p3[0] - p1[0];
  float v1 = p3[1] - p1[1];
  float v2 = p3[2] - p1[2];
  Vertex v(v0, v1, v2);

  normal[0] = (u[1] * v[2]) - (u[2] * v[1]);
  normal[1] = (u[2] * v[0]) - (u[0] * v[2]);
  normal[2] = (u[0] * v[1]) - (u[1] * v[0]);

  Normalise(normal);
}

//! Used to calculate barycentric coordinates.
//! f_ab(x, y) = (y_a - y_b)x + (x_b - x_a)y + (x_a * y_b) - (x_b * y_a)
int f(int ax, int ay, int bx, int by, int x, int y) {
  return (ay - by) * x + (bx - ax) * y + (ax * by) - (bx * ay);
}

bool InTriangle(int x, int y, Vertex p1, Vertex p2, Vertex p3) {
  float alpha;
  float beta;
  float gamma;
  CalculateBarycentricCoordinates(x, y, p1, p2, p3, alpha, beta, gamma);

  return alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1;
}

void CalculateBarycentricCoordinates(int x, int y, Vertex p1, Vertex p2,
    Vertex p3, float& alpha, float& beta, float& gamma) {
  int x0 = p1[0];
  int y0 = p1[1];

  int x1 = p2[0];
  int y1 = p2[1];

  int x2 = p3[0];
  int y2 = p3[1];

  // alpha = f_12(x,y) / f_12(x0, y0);
  alpha = (float) f(x1, y1, x2, y2, x, y) / (float) f(x1, y1, x2, y2, x0, y0);

  // beta = f_20(x,y) / f_20(x1, y1);
  beta = (float) f(x2, y2, x0, y0, x, y) / (float) f(x2, y2, x0, y0, x1, y1);

  // gamma = f_01(x,y) / f_01(x2, y2);
  gamma = (float) f(x0, y0, x1, y1, x, y) / (float) f(x0, y0, x1, y1, x2, y2);
}
}  // namespace computer_graphics
