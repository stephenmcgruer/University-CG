#!/usr/bin/python

# Fixes "broken" .obj files by making sure that all vertices that
# share the same point in 3D space are represented as a single
# vertex in the .obj file.

# Comparison screenshots found in ../images

import sys

# Unique-ifies items in aa list. Cannot just use list(set(items))
# due to list of lists (e.g. [[1,2,3],[a,b,c]]) - these are not
# hashable and cannot be made into a set.
def uniquify(items):
  unique_items = []
  for item in items:
    if item not in unique_items:
      unique_items.append(item)
  return unique_items


def main(args):
  if len(args) < 2:
    print "Usage: python fix.py object_file"
    return
  
  file = open(args[1], "r")

  vertices = []
  triangles = []

  # Parse the file.
  for line in file:
    parts = line.strip().split()

    # Skip blank lines
    if not parts:
      continue

    if parts[0] == "v":
      # Vertices are just pushed onto the 
      vertices.append([parts[1], parts[2], parts[3]])
    elif parts[0] == "f":
      # Triangle vertices are converted to their 3D
      # coordinates.
      triangle = []
      for i in range(1,4):
        vertexi = int(parts[i]) - 1
        triangle.append(vertices[vertexi])
      triangles.append(triangle)


  # Now do the output.
  vertices = uniquify(vertices)
  for vertex in vertices:
    print "v " + vertex[0] + " " + vertex[1] + " " + vertex[2]

  print ""

  for triangle in triangles:
    triangle_vertices = [vertices.index(triangle_vertex) + 1 for triangle_vertex in triangle]
    print "f " + str(triangle_vertices[0]) + " " + str(triangle_vertices[1]) + \
        " " + str(triangle_vertices[2])

if __name__ == "__main__":
  main(sys.argv)
