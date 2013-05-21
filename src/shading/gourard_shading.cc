//! \author Stephen McGruer

#include "./gourard_shading.h"

namespace computer_graphics {

//! A z-buffer approach is used to draw points in the correct order. Note that
//! multiple entries may exist in points for a single coordinate, so the drawing
//! must be done by iterating from the front of the points vector to the back.
void GourardShading::Shade(TriangleMesh object, TriangleMesh the_floor,
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

  // Gourard shading doesn't implement shadows.
  std::vector<std::vector<float> > shadow_buffer;

  RenderObject(object, window_info, light_position, view_position, z_buffer,
      points);
  RenderFloor(the_floor, window_info, light_position, view_position, z_buffer,
      shadow_buffer, points);
}

void GourardShading::RenderObject(TriangleMesh the_object,
    WindowInfo window_info, Vertex light_position, Vertex view_position,
    std::vector<std::vector<float> >& z_buffer,
    std::vector<Vertex>& points) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  // Triangle normals
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

  // Vertex normals
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

    float z = (p1[2] + p2[2] + p3[2]) / 3.0f;

    // Gourard shading calculates the phong illumination at each vertex
    // of the triangle and then interpolates.
    Vertex l1(light_position[0] - p1[0], light_position[1] - p1[1],
        light_position[2] - p1[2]);
    Normalise(l1);
    Vertex l2(light_position[0] - p2[0], light_position[1] - p2[1],
        light_position[2] - p2[2]);
    Normalise(l2);
    Vertex l3(light_position[0] - p3[0], light_position[1] - p3[1],
        light_position[2] - p3[2]);
    Normalise(l3);

    Vertex v1(view_position[0] - p1[0], view_position[1] - p1[1],
        view_position[2] - p1[2]);
    Normalise(v1);
    Vertex v2(view_position[0] - p2[0], view_position[1] - p2[1],
        view_position[2] - p2[2]);
    Normalise(v2);
    Vertex v3(view_position[0] - p3[0], view_position[1] - p3[1],
        view_position[2] - p3[2]);
    Normalise(v3);

    float ambient1;
    float diffuse1;
    float specular1;
    PhongIllumination(vertex_normals[vertices[0]], l1, v1, ambient1, diffuse1,
        specular1);
    float ambient2;
    float diffuse2;
    float specular2;
    PhongIllumination(vertex_normals[vertices[1]], l2, v2, ambient2, diffuse2,
        specular2);
    float ambient3;
    float diffuse3;
    float specular3;
    PhongIllumination(vertex_normals[vertices[2]], l3, v3, ambient3, diffuse3,
        specular3);

    float r1 = ((ambient1 + diffuse1) * red_strength()) + specular1;
    float g1 = ((ambient1 + diffuse1) * green_strength()) + specular1;
    float b1 = ((ambient1 + diffuse1) * blue_strength()) + specular1;
    clampf(r1, 0.0f, 1.0f);
    clampf(g1, 0.0f, 1.0f);
    clampf(b1, 0.0f, 1.0f);

    float r2 = ((ambient2 + diffuse2) * red_strength()) + specular2;
    float g2 = ((ambient2 + diffuse2) * green_strength()) + specular2;
    float b2 = ((ambient2 + diffuse2) * blue_strength()) + specular2;
    clampf(r2, 0.0f, 1.0f);
    clampf(g2, 0.0f, 1.0f);
    clampf(b2, 0.0f, 1.0f);

    float r3 = ((ambient3 + diffuse3) * red_strength()) + specular3;
    float g3 = ((ambient3 + diffuse3) * green_strength()) + specular3;
    float b3 = ((ambient3 + diffuse3) * blue_strength()) + specular3;
    clampf(r3, 0.0f, 1.0f);
    clampf(g3, 0.0f, 1.0f);
    clampf(b3, 0.0f, 1.0f);

    for (int y = top; y <= bottom; y++) {
      for (int x = left; x <= right; x++) {
        if (!InTriangle(x, y, p1, p2, p3) ||
            z_buffer[x + window_width / 2][y + window_height / 2] > z) {
          continue;
        }
        z_buffer[x + window_width / 2][y + window_height / 2] = z;

        float alpha;
        float beta;
        float gamma;
        CalculateBarycentricCoordinates(x, y, p1, p2, p3, alpha, beta, gamma);

        float averageRed = (alpha * r1) + (beta * r2) + (gamma * r3);
        float averageGreen = (alpha * g1) + (beta * g2) + (gamma * g3);
        float averageBlue = (alpha * b1) + (beta * b2) + (gamma * b3);

        points.push_back(Vertex(x, y, z, averageRed, averageGreen, averageBlue));
      }
    }
  }
}
}
