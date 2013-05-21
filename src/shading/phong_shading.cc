//! \author Stephen McGruer

#include "./phong_shading.h"

namespace computer_graphics {

//! A z-buffer approach is used to draw points in the correct order. Note that
//! multiple entries may exist in points for a single coordinate, so the drawing
//! must be done by iterating from the front of the points vector to the back.
void PhongShading::Shade(TriangleMesh object, TriangleMesh the_floor,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<Vertex>& points, IplImage* image) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  // Calculate the shadows.
  std::vector<std::vector<float> > shadow_buffer;
  if (shadows()) {
    CalculateShadowBuffer(object, window_info, light_position, shadow_buffer);
    CalculateShadowBuffer(the_floor, window_info, light_position, shadow_buffer);
  }

  // Initialise the z-buffer.
  std::vector<std::vector<float>  > z_buffer;
  for (int i = 0; i <= window_width; i++) {
    std::vector<float> line(window_height + 1, -1000.0f);
    z_buffer.push_back(line);
  }

  RenderObject(object, window_info, light_position, view_position, z_buffer,
      shadow_buffer, points);
  RenderFloor(the_floor, window_info, light_position, view_position, z_buffer,
      shadow_buffer, points);
}

void PhongShading::CalculateShadowBuffer(TriangleMesh the_object,
    WindowInfo window_info, Vertex light_position,
    std::vector<std::vector<float> >& shadow_buffer) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  // Initialise the shadow buffer if necessary.
  if (shadow_buffer.empty()) {
    for (int i = 0; i <= window_width; i++) {
      std::vector<float> line(window_height + 1, -1000.0f);
      shadow_buffer.push_back(line);
    }
  }

  for (int i = 0; i < the_object.trigNum(); i++) {
    std::vector<int> vertices;
    the_object.GetTriangleVerticesInt(i, vertices);
    Vertex p1 = the_object.v(vertices[0]);
    Vertex p2 = the_object.v(vertices[1]);
    Vertex p3 = the_object.v(vertices[2]);

    // Orthogonally project to the light's viewpoint.
    Project(p1, light_position, window_width, window_height);
    Project(p2, light_position, window_width, window_height);
    Project(p3, light_position, window_width, window_height);

    // Establish a bounding box for the triangle.
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
        if (!InTriangle(x, y, p1, p2, p3)) {
          continue;
        }
        float alpha;
        float beta;
        float gamma;
        CalculateBarycentricCoordinates(x, y, p1, p2, p3, alpha, beta, gamma);
        float z = alpha * p1[2] + beta * p2[2] + gamma * p3[2];

        if (shadow_buffer[x + window_width / 2][y + window_height / 2] > z) {
          continue;
        }
        shadow_buffer[x + window_width / 2][y + window_height / 2] = z;
      }
    }
  }
}

void PhongShading::RenderObject(TriangleMesh the_object,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<std::vector<float> >& z_buffer,
    std::vector<std::vector<float> > shadow_buffer,
    std::vector<Vertex>& points) {
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

        float ambient;
        float diffuse;
        float specular;
        PhongIllumination(point_normal, light, view, ambient, diffuse, specular);

        float red;
        float green;
        float blue;
        if (!shadows()) {
          // Not using shadows.
          red = ((ambient + diffuse) * red_strength()) + specular;
          green = ((ambient + diffuse) * green_strength()) + specular;
          blue = ((ambient + diffuse) * blue_strength()) + specular;
        } else {
          // Using shadows.

          // Project the point to the light's viewpoint.
          Vertex point(x, y, z);
          Project(point, light_position, window_width, window_height);
          float projX = point[0];
          float projY = point[1];
          float projZ = point[2];

          if ((projX + window_width / 2) < 0 ||
              (projX + window_width / 2) > shadow_buffer.size() ||
              (projY + window_height / 2) < 0 ||
              (projY + window_height / 2) > shadow_buffer[0].size() ||
              shadow_buffer[projX + window_width / 2][projY + window_height / 2] <= (projZ + 10.0f)) {
            // If the point is outside what the light can see, or if it is the
            // closest thing the point can see, then it is lit.
            red = ((ambient + diffuse) * red_strength()) + specular;
            green = ((ambient + diffuse) * green_strength()) + specular;
            blue = ((ambient + diffuse) * blue_strength()) + specular;
          } else {
            // The point is in shadow.
            red = ambient * red_strength();
            green = ambient * green_strength();
            blue = ambient * blue_strength();
          }
        }
        clampf(red, 0.0f, 1.0f);
        clampf(green, 0.0f, 1.0f);
        clampf(blue, 0.0f, 1.0f);

        points.push_back(Vertex(x, y, z, red, green, blue));
      }
    }
  }
}
}
