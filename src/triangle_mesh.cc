//! \author Stephen McGruer

#include "./triangle_mesh.h"

namespace computer_graphics {

void TriangleMesh::LoadFile(const char * filename, bool scale) {
  FILE *f;
  f = fopen(filename, "r");

  if (f == NULL) {
    fprintf(stderr, "Failed reading polygon data file %s\n", filename);
    exit(1);
  }

  char prevBuf = '0';
  char buf[1024];
  char header[100];
  float x, y, z;
  float xmax, ymax, zmax, xmin, ymin, zmin;
  int v1, v2, v3;

  if (scale) {
    xmax =-10000;
    ymax =-10000;
    zmax =-10000;
    xmin =10000;
    ymin =10000;
    zmin =10000;
  }

  Vertex av;

  while (fgets(buf, sizeof(buf), f) != NULL) {
    if(buf[0] == 'v') {
      sscanf(buf, "%s %f %f %f", header, &x, &y, &z);

      mesh_vertices_.push_back(Vertex(x, y, z));

      if (scale) {
        av[0] += x;
        av[1] += y;
        av[2] += z;

        if (x > xmax) {
          xmax = x;
        }
        if (y > ymax) {
          ymax = y;
        }
        if (z > zmax) {
          zmax = z;
        }

        if (x < xmin) xmin = x;
        if (y < ymin) ymin = y;
        if (z < zmin) zmin = z;
      }
    } else if (buf[0] == 'f') {
      if (buf[0] != prevBuf) {
        // Set up space for the vertex-triangle linker
        for (int i = 0; i < vNum(); i++) {
          std::vector<int> triangle;
          vertices_to_triangles_.push_back(triangle);
        }
      }
      sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);

      Triangle trig(v1-1, v2-1, v3-1);
      mesh_triangles_.push_back(trig);
      vertices_to_triangles_[v1 - 1].push_back(mesh_triangles_.size() - 1);
      vertices_to_triangles_[v2 - 1].push_back(mesh_triangles_.size() - 1);
      vertices_to_triangles_[v3 - 1].push_back(mesh_triangles_.size() - 1);
    }
    prevBuf = buf[0];
  }

  if (scale) {
    float range;
    if (xmax-xmin > ymax-ymin) {
      range = xmax-xmin;
    } else {
      range = ymax-ymin;
    }

    for (int j = 0; j < 3; j++) av[j] /= mesh_vertices_.size();

    for (int i = 0; i < static_cast<int>(mesh_vertices_.size()); i++) {
      for (int j = 0; j < 3; j++) {
        mesh_vertices_[i][j] = (mesh_vertices_[i][j]-av[j])/range*400;
      }
    }
  }

  printf("Trig %i vertices %i\n", static_cast<int>(mesh_triangles_.size()),
      static_cast<int>(mesh_vertices_.size()));
  fclose(f);
}

void TriangleMesh::GetTriangleVertices(int index, Vertex &v1, Vertex &v2,
    Vertex & v3) {
  v1 = mesh_vertices_[mesh_triangles_[index].triangle_vertices_[0]];
  v2 = mesh_vertices_[mesh_triangles_[index].triangle_vertices_[1]];
  v3 = mesh_vertices_[mesh_triangles_[index].triangle_vertices_[2]];
}

void TriangleMesh::GetTriangleVerticesInt(int index, std::vector<int>& vertices) {
  vertices.push_back(mesh_triangles_[index].triangle_vertices_[0]);
  vertices.push_back(mesh_triangles_[index].triangle_vertices_[1]);
  vertices.push_back(mesh_triangles_[index].triangle_vertices_[2]);
}

TriangleMesh& TriangleMesh::ApplyTransformation(
    FloatMatrix transformation_matrix) {
  // All transformation matrices must be 4*4.
  if (transformation_matrix.num_cols() != 4
      || transformation_matrix.num_rows() != 4) {
    fprintf(stderr, "Error: Input matrix wrong size.");
    return *this;
  }

  // Have to move teapot to origin, apply transformation, move back.
  float middle_x = 0;
  float middle_y = 0;
  float middle_z = 0;
  for (int i = 0; i < vNum(); i++) {
    middle_x += mesh_vertices_[i][0];
    middle_y += mesh_vertices_[i][1];
    middle_z += mesh_vertices_[i][2];
  }
  middle_x /= vNum();
  middle_y /= vNum();
  middle_z /= vNum();

  for (std::vector<Vertex>::iterator it = mesh_vertices_.begin();
      it != mesh_vertices_.end(); it++) {
    FloatMatrix vertex_matrix(4, 1);
    vertex_matrix(0, 0) = (*it)[0] - middle_x;
    vertex_matrix(0, 1) = (*it)[1] - middle_y;
    vertex_matrix(0, 2) = (*it)[2] - middle_z;
    vertex_matrix(0, 3) = 1.0f;

    FloatMatrix result = transformation_matrix * vertex_matrix;

    (*it).set_x((result(0, 0) / result(0, 3)) + middle_x);
    (*it).set_y((result(0, 1) / result(0, 3)) + middle_y);
    (*it).set_z((result(0, 2) / result(0, 3)) + middle_z);
  }

  return *this;
}

}  // namespace computer_graphics
