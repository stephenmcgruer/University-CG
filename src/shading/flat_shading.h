//! \author Stephen McGruer

#ifndef SRC_SHADING_FLATSHADING_H_
#define SRC_SHADING_FLATSHADING_H_

#include "./shading_algorithm.h"

namespace computer_graphics {

//! \class FlatShading
//! \brief Shades a scene using the flat shading approach.
class FlatShading : public ShadingAlgorithm {
  public:
    inline FlatShading() { };

    //! \brief Calculates the shading for each visible triangle in the mesh, and
    //!        places it in the points variable.
    //!
    //! Each pixel in a triangle is shaded using the triangle's normal and using
    //! the centroid of the triangle to determine the light and view vectors.
    //!
    //! The image variable is ignored.
    void Shade(TriangleMesh object, TriangleMesh the_floor, WindowInfo window_info,
        Vertex light_position, Vertex view_position, std::vector<Vertex>& points,
        IplImage* image = NULL);

  private:
    //! \brief Renders an object in the scene.
    //!
    //! Expects the z-buffer to be already initialised. Takes control of it during
    //! the function, and will update it.
    void RenderObject(TriangleMesh the_object, WindowInfo window_info,
        Vertex light_position, Vertex view_position,
        std::vector<std::vector<float> >& z_buffer,
        std::vector<Vertex>& points);
};
}

#endif // SRC_SHADING_FLATSHADING_H_
