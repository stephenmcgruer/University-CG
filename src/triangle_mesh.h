//! \author Stephen McGruer

#ifndef SRC_TRIANGLEMESH_H_
#define SRC_TRIANGLEMESH_H_

#include <vector>
#include <cstdio>
#include <cstdlib>

#include "./float_matrix.h"
#include "./triangle.h"
#include "./vertex.h"

namespace computer_graphics {

//! \class TriangleMesh
//! \brief Represents a polygon implemented as a mesh of triangles.
class TriangleMesh {
  public:
    explicit TriangleMesh(char * filename) {
      LoadFile(filename);
    }

    TriangleMesh() {
    }

    //! \brief Loads in an object file and populates the mesh from it.
    //!
    //! If the scale variable is set to false, the points read in from
    //! the object file will be treated as exact window coordinates and
    //! will not be scaled.
    void LoadFile(const char *filename, bool scale = true);

    //! \brief Returns the three vertices of a triangle.
    void GetTriangleVertices(int index, Vertex& v1, Vertex& v2, Vertex& v3);

    //! \brief Returns the indices of the three vertices of a triangle.
    void GetTriangleVerticesInt(int index, std::vector<int>&);

    //! \brief Applies a transformation matrix to the mesh points.
    //!
    //! Return this object, in order to facilitate chaining.
    TriangleMesh& ApplyTransformation(FloatMatrix transformation_matrix);

    inline int trigNum() {
      return mesh_triangles_.size();
    }
    inline int vNum() {
      return mesh_vertices_.size();
    }
    inline Vertex v(int i) {
      return mesh_vertices_[i];
    }

    //! \brief Returns the set of triangles that a vertex belongs to.
    inline std::vector<int> GetTrianglesForVertex(int v) {
      return vertices_to_triangles_[v];
    }
  private:
    std::vector<Vertex> mesh_vertices_;
    std::vector<Triangle> mesh_triangles_;
    std::vector<std::vector<int> > vertices_to_triangles_;
};
}

#endif  // SRC_TRIANGLEMESH_H_
