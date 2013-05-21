//! \author Stephen McGruer

#ifndef SRC_SHADING_SHADINGALGORITHM_H_
#define SRC_SHADING_SHADINGALGORITHM_H_

#include "./shading_utils.h"
#include "../teapot_utils.h"
#include "../triangle_mesh.h"

#include <vector>

namespace computer_graphics {

//! \class ShadingAlgorithm
//! \brief Represents a generic shading algorithm.
//!
//! Presents an interface for a generic shading algorithm, and provides
//! a RenderFloor function for rendering the floor in a scene.
class ShadingAlgorithm {
  public:
    ShadingAlgorithm()
        : k_a_(0.2f),
          k_d_(0.5f),
          k_s_(0.7f),
          alpha_(50.0f),
          i_a_(0.4f),
          i_d_(0.5f),
          i_s_(1.0f),
          red_strength_(1.0f),
          green_strength_(0.0f),
          blue_strength_(0.0f) {
      floor_texture_ = cvLoadImage("textures/floor.jpg", CV_LOAD_IMAGE_COLOR);
    }

    //! \brief Calculates the shading for a scene.
    //!
    //! The calculated shading is placed in the points variable.
    virtual void Shade(TriangleMesh object, TriangleMesh the_floor, WindowInfo window_info,
        Vertex light_position, Vertex view_position, std::vector<Vertex>& points,
        IplImage* image = NULL) = 0;

    //! \brief Renders the floor in the scene.
    //!
    //! This differs from other objects as it does not use the full Phong
    //! Illumination.
    //!
    //! Expects the z-buffer to be already initialised. Takes control of it during
    //! the function, and will update it.
    //!
    //! If the shadow_buffer is not empty, will use it to attempt to render
    //! shadows as well.
    void RenderFloor(TriangleMesh the_floor, WindowInfo window_info,
        Vertex light_position, Vertex view_position,
        std::vector<std::vector<float> >& z_buffer,
        std::vector<std::vector<float> > shadow_buffer,
        std::vector<Vertex>& points);

    //! \brief Calculates the Phong illumination for a given normal, light vector, and
    //!        view vector.
    void PhongIllumination(Vertex normal, Vertex light, Vertex view, float& ambient,
        float& diffuse, float& specular);

    //! \brief Clamps a value between min and max.
    inline int clamp(int value, int min, int max) {
      return std::min(max, std::max(value, min));
    }

    //! \brief Clamps a value between min and max.
    inline float clampf(float value, float min, float max) {
      return std::min(max, std::max(value, min));
    }

    inline float& k_a() { return k_a_; }
    inline float& k_d() { return k_d_; }
    inline float& k_s() { return k_s_; }
    inline float& alpha() { return alpha_; }
    inline float& i_a() { return i_a_; }
    inline float& i_d() { return i_d_; }
    inline float& i_s() { return i_s_; }

    inline float& red_strength() { return red_strength_; }
    inline float& green_strength() { return green_strength_; }
    inline float& blue_strength() { return blue_strength_; }

    inline bool shadows() { return shadows_; }

    inline void ToggleShadows() { shadows_ = (shadows_) ? false : true; }
  private:
    IplImage* floor_texture_;

    // Shading constants.
    float k_a_;
    float k_d_;
    float k_s_;
    float alpha_;
    float i_a_;
    float i_d_;
    float i_s_;

    // Colour strengths.
    float red_strength_;
    float green_strength_;
    float blue_strength_;

    // Shadows
    bool shadows_;
};
}

#endif // SRC_SHADING_SHADINGALGORITHM_H_
