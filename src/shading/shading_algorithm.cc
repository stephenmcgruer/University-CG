//! \author Stephen McGruer

#include "./shading_algorithm.h"

namespace computer_graphics {

//! Uses a Phong/Gourard shading-like approach, in order to get
//! nice z-interpolation for the z_buffer.
void ShadingAlgorithm::RenderFloor(TriangleMesh the_floor, WindowInfo window_info,
    Vertex light_position, Vertex view_position,
    std::vector<std::vector<float> >& z_buffer,
    std::vector<std::vector<float> > shadow_buffer,
    std::vector<Vertex>& points) {
  int window_width = std::abs(window_info.left) + std::abs(window_info.right);
  int window_height = std::abs(window_info.top) + std::abs(window_info.bottom);

  for (int i = 0; i < the_floor.trigNum(); i++) {
    std::vector<int> vertices;
    the_floor.GetTriangleVerticesInt(i, vertices);
    Vertex p1 = the_floor.v(vertices[0]);
    Vertex p2 = the_floor.v(vertices[1]);
    Vertex p3 = the_floor.v(vertices[2]);

    // For efficiency, establish a bounding box for the triangle.
    int left = clamp(std::min(p1[0], std::min(p2[0], p3[0])),
        window_info.left, window_info.right);
    int right = clamp(std::max(p1[0], std::max(p2[0], p3[0])),
        window_info.left, window_info.right);
    int top = clamp(std::min(p1[1], std::min(p2[1], p3[1])),
        window_info.top, window_info.bottom);
    int bottom = clamp(std::max(p1[1], std::max(p2[1], p3[1])),
        window_info.top, window_info.bottom);

    // Render the floor triangles.
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

        if (z_buffer[x + window_width / 2][y + window_height / 2] > z) {
          continue;
        }
        z_buffer[x + window_width / 2][y + window_height / 2] = z;

        // Fit x,y to image-width/image-height
        int fitted_x = ((float) (x + window_width / 2) / window_width) * floor_texture_->width;
        int fitted_y = ((float) (y + window_height / 2) / window_height) * floor_texture_->height;

        // The data is stored BGR not RGB.
        std::vector<float> colours;
        uchar *data;
        data = (uchar *) floor_texture_->imageData;
        if (!shadows_) {
          colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 2] / 255.0f);
          colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 1] / 255.0f);
          colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 0] / 255.0f);
        } else {
          Vertex point(x, y, z);
          Project(point, light_position, window_width, window_height);
          float projX = point[0];
          float projY = point[1];
          float projZ = point[2];

          if ((projX + window_width / 2) < 0 ||
              (projX + window_width / 2) > shadow_buffer.size() ||
              (projY + window_height / 2) < 0 ||
              (projY + window_height / 2) > shadow_buffer[0].size() ||
              shadow_buffer[projX + window_width / 2][projY + window_height / 2] <= projZ) {
            // If the point is outside what the light can see, or if it is the
            // closest thing the point can see, then it is lit.
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 2] / 255.0f);
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 1] / 255.0f);
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 0] / 255.0f);
          } else {
            // The point is in shadow.
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 2] / 255.0f - 0.5f);
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 1] / 255.0f - 0.5f);
            colours.push_back((float) data[fitted_y * floor_texture_->widthStep + fitted_x * floor_texture_->nChannels + 0] / 255.0f - 0.5f);
          }
        }
        clampf(colours[0], 0.0f, 1.0f);
        clampf(colours[1], 0.0f, 1.0f);
        clampf(colours[2], 0.0f, 1.0f);

        points.push_back(Vertex(x, y, z, colours[0], colours[1], colours[2]));
      }
    }
  }
}

void ShadingAlgorithm::PhongIllumination(Vertex normal, Vertex light, Vertex view, float& ambient,
    float& diffuse, float& specular) {
  // reflection = 2(light . normal)normal - light;

  float constant = 2 * DotProduct(light, normal);
  Vertex reflection;
  reflection[0] = constant * normal[0] - light[0];
  reflection[1] = constant * normal[1] - light[1];
  reflection[2] = constant * normal[2] - light[2];

  Vertex halfway;
  halfway[0] = light[0] + view[0];
  halfway[1] = light[1] + view[1];
  halfway[2] = light[2] + view[2];
  Normalise(halfway);

  ambient = k_a_ * i_a_;
  diffuse = k_d_ * i_d_ * std::max(0.0f, DotProduct(normal, light));
  specular = k_s_ * i_s_ * std::max(0.0f, std::pow(DotProduct(normal, halfway), alpha_));
}
}
