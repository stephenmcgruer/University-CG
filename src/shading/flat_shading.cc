//! \author Stephen McGruer

#include "./flat_shading.h"

namespace computer_graphics {

//! A z-buffer approach is used to draw points in the correct order. Note that
//! multiple entries may exist in points for a single coordinate, so the drawing
//! must be done by iterating from the front of the points vector to the back.
void FlatShading::Shade(TriangleMesh object, TriangleMesh the_floor,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<Vertex>& points, IplImage* image) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  // Initialise the z-buffer.
  std::vector<std::vector<float>  > z_buffer;
  for (int i = 0; i <= window_width; i++) {
    std::vector<float> line(window_height + 1, -1000.0f);
    z_buffer.push_back(line);
  }

  // Flat shading doesn't implement shadows.
  std::vector<std::vector<float> > shadow_buffer;

  RenderObject(object, window_info, light_position, view_position, z_buffer,
      points);
  RenderFloor(the_floor, window_info, light_position, view_position, z_buffer,
      shadow_buffer, points);
}

void FlatShading::RenderObject(TriangleMesh the_object,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<std::vector<float> >& z_buffer,
    std::vector<Vertex>& points) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  Vertex p1;
  Vertex p2;
  Vertex p3;
  for (int i = 0; i < the_object.trigNum(); i++) {
    the_object.GetTriangleVertices(i, p1, p2, p3);

    Vertex normal;
    ComputeSurfaceNormal(p1, p2, p3, normal);

    // Now we need to project them.
    Project(p1, view_position, window_width, window_height);
    Project(p2, view_position, window_width, window_height);
    Project(p3, view_position, window_width, window_height);

    // For efficiency, establish a bounding box for the triangle.
    int left = clamp(std::min(p1[0], std::min(p2[0], p3[0])),
        window_info.left, window_info.right);
    int right = clamp(std::max(p1[0], std::max(p2[0], p3[0])),
        window_info.left, window_info.right);
    int top = clamp(std::min(p1[1], std::min(p2[1], p3[1])),
        window_info.top, window_info.bottom);
    int bottom = clamp(std::max(p1[1], std::max(p2[1], p3[1])),
        window_info.top, window_info.bottom);

    // Flat shading computes shading information based on the centroid
    // of the triangle.
    float centre_x = (p1[0] + p2[0] + p3[0]) / 3.0f;
    float centre_y = (p1[1] + p2[1] + p3[1]) / 3.0f;
    float z = (p1[2] + p2[2] + p3[2]) / 3.0f;

    Vertex light(light_position[0] - centre_x, light_position[1] - centre_y, light_position[2] - z);
    Normalise(light);

    Vertex view(view_position[0] - centre_x, view_position[1] - centre_y, view_position[2] - z);
    Normalise(view);

    float ambient;
    float diffuse;
    float specular;
    PhongIllumination(normal, light, view, ambient, diffuse, specular);

    float red = ((ambient + diffuse) * red_strength()) + specular;
    float green = ((ambient + diffuse) * green_strength()) + specular;
    float blue = ((ambient + diffuse) * blue_strength()) + specular;

    clampf(red, 0.0f, 1.0f);
    clampf(green, 0.0f, 1.0f);
    clampf(blue, 0.0f, 1.0f);

    for (int y = top; y <= bottom; y++) {
      for (int x = left; x <= right; x++) {
        if (!InTriangle(x, y, p1, p2, p3) ||
            z_buffer[x + window_width / 2][y + window_height / 2] > z) {
          continue;
        }
        z_buffer[x + window_width / 2][y + window_height / 2] = z;

        points.push_back(Vertex(x, y, z, red, green, blue));
      }
    }
  }
}
}
