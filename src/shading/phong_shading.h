//! \author Stephen McGruer

#ifndef SRC_SHADING_PHONGSHADING_H_
#define SRC_SHADING_PHONGSHADING_H_

#include "./shading_algorithm.h"

namespace computer_graphics {

//! \class PhongShading
//! \brief Shades a scene using the Phong shading approach.
class PhongShading : public ShadingAlgorithm {
  public:
    inline PhongShading() { };

    //! \brief Calculates the shading for each visible triangle in the mesh, and
    //!        places it in the points variable.
    //!
    //! Each pixel in a triangle is shaded by calculating the normals at each of
    //! the triangle's vertices, then interpolating the three normals for each
    //! point.
    //!
    //! The image variable is ignored.
    void Shade(TriangleMesh object, TriangleMesh the_floor, WindowInfo window_info,
        Vertex light_position, Vertex view_position, std::vector<Vertex>& points,
        IplImage* image = NULL);

  private:
    //! \brief Calculates the shadow buffer for an arbitrary object.
    //!
    //! If the shadow buffer is already initialised, it is preserved and updated
    //! in the function.
    void CalculateShadowBuffer(TriangleMesh the_object, WindowInfo window_info,
        Vertex light_position, std::vector<std::vector<float> >& shadow_buffer);

    //! \brief Renders an object in the scene.
    //!
    //! Expects the z-buffer to be already initialised. Takes control of it during
    //! the function, and will update it.
    //!
    //! If the shadow_buffer is not empty, will use it to attempt to render
    //! shadows as well.
    void RenderObject(TriangleMesh the_object, WindowInfo window_info,
        Vertex light_position, Vertex view_position,
        std::vector<std::vector<float> >& z_buffer,
        std::vector<std::vector<float> > shadow_buffer,
        std::vector<Vertex>& points);
};
}

#endif // SRC_SHADING_PHONGSHADING_H_
