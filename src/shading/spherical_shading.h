//! \author Stephen McGruer

#ifndef SRC_SHADING_SPHERICALSHADING_H_
#define SRC_SHADING_SPHERICALSHADING_H_

#include "./shading_algorithm.h"

namespace computer_graphics {

//! \class SphericalShading
//! \brief Shades a scene using a spherical environment map.
class SphericalShading : public ShadingAlgorithm {
  public:
    inline SphericalShading() { };

    //! \brief Calculates the shading for each visible triangle in the mesh, and
    //!        places it in the points variable.
    //!
    //! Each pixel in a triangle is shaded using a spherical environment map given in
    //! the image variable.
    void Shade(TriangleMesh object, TriangleMesh the_floor, WindowInfo window_info,
        Vertex light_position, Vertex view_position, std::vector<Vertex>& points,
        IplImage* image);

  private:
    //! \brief Renders an object in the scene.
    //!
    //! Expects the z-buffer to be already initialised. Takes control of it during
    //! the function, and will update it.
    void RenderObject(TriangleMesh the_object, WindowInfo window_info,
        Vertex light_position, Vertex view_position,
        std::vector<std::vector<float> >& z_buffer,
        std::vector<Vertex>& points, IplImage* image);

    //! \brief Calculates the colour for a given normal, light vector and view vector,
    //!        based on a spherical environment map found in image.
    //!
    //! Pushes the resultant RGB colours onto the colour variable.
    void SphericalEnvironmentMap(Vertex normal, Vertex light, Vertex view,
        std::vector<float>& colour, IplImage* image);
};
}

#endif // SRC_SHADING_SPHERICALSHADING_H_
