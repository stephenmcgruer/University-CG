#
# Makefile for Computer Graphics coursework
#

CC=gcc
CFLAGS=-Wall -lglut -lGLU -lGL

teapot :
	mkdir -p bin/src/shading
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/mouse_loc.o src/mouse_loc.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/spherical_shading.o src/shading/spherical_shading.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/gourard_shading.o src/shading/gourard_shading.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/phong_shading.o src/shading/phong_shading.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/shading_utils.o src/shading/shading_utils.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/teapot.o src/teapot.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/shading_algorithm.o src/shading/shading_algorithm.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/vertex.o src/vertex.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/shading/flat_shading.o src/shading/flat_shading.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/triangle_mesh.o src/triangle_mesh.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/float_matrix.o src/float_matrix.cc
	g++ -I/usr/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -obin/src/teapot_utils.o src/teapot_utils.cc
	g++ -L/usr/local/lib -obin/teapot bin/src/vertex.o bin/src/triangle_mesh.o bin/src/teapot_utils.o bin/src/teapot.o bin/src/shading/spherical_shading.o bin/src/shading/shading_utils.o bin/src/shading/shading_algorithm.o bin/src/shading/phong_shading.o bin/src/shading/gourard_shading.o bin/src/shading/flat_shading.o bin/src/mouse_loc.o bin/src/float_matrix.o -lglut -lcv -lhighgui -lGLU


doxygen :
	doxygen Doxyfile

clean : 
	rm -rf bin 
	rm -f src/Teapot.o src/TriangleMesh.o

