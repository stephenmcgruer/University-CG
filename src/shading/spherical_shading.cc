//! \author Stephen McGruer

#include "./spherical_shading.h"

namespace computer_graphics {

//! A z-buffer approach is used to draw points in the correct order. Note that
//! multiple entries may exist in points for a single coordinate, so the drawing
//! must be done by iterating from the front of the points vector to the back.
void SphericalShading::Shade(TriangleMesh object, TriangleMesh the_floor,
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

  // Spherical environment mapping doesn't implement shadows.
  std::vector<std::vector<float> > shadow_buffer;

  RenderObject(object, window_info, light_position, view_position, z_buffer,
      points, image);
  RenderFloor(the_floor, window_info, light_position, view_position, z_buffer,
      shadow_buffer, points);
}

void SphericalShading::RenderObject(TriangleMesh the_object,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<std::vector<float> >& z_buffer,
    std::vector<Vertex>& points, IplImage* image) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  // Compute the triangle normals.
  std::vector<Vertex> triangle_normals;
  for (int i = 0; i < the_object.trigNum(); i++) {
    Vertex p1;
    Vertex p2;
    Vertex p3;
    Vertex trNormal;
    the_object.GetTriangleVertices(i, p1, p2, p3);
    ComputeSurfaceNormal(p1, p2, p3, trNormal);
    triangle_normals.push_back(trNormal);
  }

  // Compute the vertex normals
  std::vector<Vertex> vertex_normals;
  for (int i = 0; i < the_object.vNum(); i++) {
    Vertex vNormal;
    std::vector<int> triangles = the_object.GetTrianglesForVertex(i);
    for (std::vector<int>::iterator it = triangles.begin();
        it != triangles.end(); it++) {
      vNormal[0] += triangle_normals[(*it)][0];
      vNormal[1] += triangle_normals[(*it)][1];
      vNormal[2] += triangle_normals[(*it)][2];
    }
    vNormal[0] /= triangles.size();
    vNormal[1] /= triangles.size();
    vNormal[2] /= triangles.size();

    vertex_normals.push_back(vNormal);
  }

  // Render the triangles in the object.
  for (int i = 0; i < the_object.trigNum(); i++) {
    std::vector<int> vertices;
    the_object.GetTriangleVerticesInt(i, vertices);
    Vertex p1 = the_object.v(vertices[0]);
    Vertex p2 = the_object.v(vertices[1]);
    Vertex p3 = the_object.v(vertices[2]);

    // For efficiency, establish a bounding box for the triangle.
    int left = clamp(std::min(p1[0], std::min(p2[0], p3[0])),
        window_info.left, window_info.right);
    int right = clamp(std::max(p1[0], std::max(p2[0], p3[0])),
        window_info.left, window_info.right);
    int top = clamp(std::min(p1[1], std::min(p2[1], p3[1])),
        window_info.top, window_info.bottom);
    int bottom = clamp(std::max(p1[1], std::max(p2[1], p3[1])),
        window_info.top, window_info.bottom);

    for (int y = top; y <= bottom; y++) {
      for (int x = left; x <= right; x++) {
        // Skip non-triangle pixels.
        if (!InTriangle(x, y, p1, p2, p3)) {
          continue;
        }
        float alpha;
        float beta;
        float gamma;
        CalculateBarycentricCoordinates(x, y, p1, p2, p3, alpha, beta, gamma);
        float z = alpha * p1[2] + beta * p2[2] + gamma * p3[2];

        // Skip hidden pixels.
        if (z_buffer[x + window_width / 2][y + window_height / 2] > z) {
          continue;
        }
        z_buffer[x + window_width / 2][y + window_height / 2] = z;

        // Interpolate the normal vector for the point from the vertex normals.
        Vertex point_normal;
        point_normal[0] = (alpha * vertex_normals[vertices[0]][0]) +
            (beta * vertex_normals[vertices[1]][0]) +
            (gamma * vertex_normals[vertices[2]][0]);
        point_normal[1] = (alpha * vertex_normals[vertices[0]][1]) +
            (beta * vertex_normals[vertices[1]][1]) +
            (gamma * vertex_normals[vertices[2]][1]);
        point_normal[2] = (alpha * vertex_normals[vertices[0]][2]) +
            (beta * vertex_normals[vertices[1]][2]) +
            (gamma * vertex_normals[vertices[2]][2]);

        Vertex light(light_position[0] - x, light_position[1] - y,
            light_position[2] - z);
        Normalise(light);

        Vertex view(view_position[0] -  x, view_position[1] - y,
            view_position[2] - z);
        Normalise(view);

        std::vector<float> colour;
        SphericalEnvironmentMap(point_normal, light, view, colour, image);

        points.push_back(Vertex(x, y, z, colour[0], colour[1], colour[2]));
      }
    }
  }
}

void SphericalShading::SphericalEnvironmentMap(Vertex normal, Vertex light, Vertex view,
    std::vector<float>& colour, IplImage* image) {
  // reflection = 2(light . normal)normal - light;
  float constant = 2 * DotProduct(light, normal);
  Vertex reflection;
  reflection[0] = constant * normal[0] - light[0];
  reflection[1] = constant * normal[1] - light[1];
  reflection[2] = constant * normal[2] - light[2];
  Normalise(reflection);

  // m = sqrt(Rx^2 + Ry^2 + (Rz + 1)^2)
  // u = (Rx / 2m) + 1/2
  // v = (Ry / 2m) + 1/2
  float m = std::sqrt(
      std::pow(reflection[0], 2) +
      std::pow(reflection[1], 2) +
      std::pow(reflection[2]  + 1, 2));
  int u = (reflection[0] / (2 * m) + 0.5f) * image->height;
  int v = (1 - (reflection[1] / (2 * m) + 0.5f)) * image->width;

  // The imagedata is stored BGR, not RGB.
  uchar *data;
  data = (uchar *) image->imageData;
  colour.push_back((float) data[v * image->widthStep + u * image->nChannels + 2] / 255.0f);
  colour.push_back((float) data[v * image->widthStep + u * image->nChannels + 1] / 255.0f);
  colour.push_back((float) data[v * image->widthStep + u * image->nChannels + 0] / 255.0f);
}
}
