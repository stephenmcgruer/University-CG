//! \author Stephen McGruer

//! The main runner file. Handles the OpenGL calls.

#include <GL/glut.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "./mouse_loc.h"
#include "./triangle_mesh.h"
#include "./shading/shading_algorithm.h"
#include "./shading/phong_shading.h"
#include "./shading/gourard_shading.h"
#include "./shading/flat_shading.h"
#include "./shading/spherical_shading.h"

namespace cg = computer_graphics;

const int kWindowWidth = 640;
const int kWindowHeight = 480;

int current_button;
cg::MouseLoc old_mouse_location;
cg::WindowInfo window_info(-kWindowWidth/2, kWindowWidth/2,
    -kWindowHeight/2, kWindowHeight/2);
bool aa = false;

cg::TriangleMesh the_object;
cg::TriangleMesh the_floor;

// The possible shading algorithms.
cg::ShadingAlgorithm* shading_algorithm;
cg::PhongShading phong_shading;
cg::GourardShading gourard_shading;
cg::FlatShading flat_shading;
cg::SphericalShading spherical_shading;

// The texture map used for spherical environment mapping.
IplImage* spherical_texture_map;

// The light and viewpoints.
cg::Vertex light(75.0f, 75.0f, 0.0f);
cg::Vertex view(0.0f, 0.0f, 0.0f);

// Forward declarations.
void display();
void mouseDragged(int, int);
void mouseClicked(int, int, int, int);
void keyboard(unsigned char, int, int);

int main(int argc, char **argv) {
  char* filename;
  if (argc == 2) {
    // The default shading algorithm is Phong Shading.
    shading_algorithm = &phong_shading;
    filename = argv[1];
  } else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
    if (strcmp(argv[2], "Flat") == 0) {
      shading_algorithm = &flat_shading;
    } else if (strcmp(argv[2], "Gourard") == 0) {
      shading_algorithm = &gourard_shading;
    } else if (strcmp(argv[2], "Phong") == 0) {
      shading_algorithm = &phong_shading;
    } else if (strcmp(argv[2], "Spherical") == 0) {
      shading_algorithm = &spherical_shading;
      spherical_texture_map =
          cvLoadImage("textures/gl_map.jpg", CV_LOAD_IMAGE_COLOR);
    } else {
      fprintf(stderr, "Error: Unrecognized algorithm '%s'.\n\n", argv[2]);
      fprintf(stderr, "Possible shading algorithms are:\n");
      fprintf(stderr, "    Flat\n");
      fprintf(stderr, "    Gourard\n");
      fprintf(stderr, "    Phong\n");
      fprintf(stderr, "    Spherical\n");

      return 1;
    }
    filename = argv[3];
  } else {
    fprintf(stderr, "Usage: %s [-s shading_algorithm] filename \n\n", argv[0]);
    fprintf(stderr, "Possible shading algorithms are:\n");
    fprintf(stderr, "    Flat\n");
    fprintf(stderr, "    Gourard\n");
    fprintf(stderr, "    Phong\n");
    fprintf(stderr, "    Spherical\n");
    return 1;
  }
  the_object.LoadFile(filename);
  the_floor.LoadFile("objects/floor.obj", false);

  // The object and floor must be moved "back" in the scene,
  // as the view is at (0,0,0).
  cg::FloatMatrix f(4, 4);
  cg::CreateMovMatrix(f, 0, 0, -500);
  the_object.ApplyTransformation(f);
  the_floor.ApplyTransformation(f);

  // The floor must also be moved so that it lies below the object.
  if (the_object.vNum() > 0) {
    float miny = the_object.v(0)[1];
    for (int i = 1; i < the_object.vNum(); i++) {
      float y = the_object.v(i)[1];
      if (y < miny) {
        miny = y;
      }
    }
    cg::FloatMatrix i(4, 4);
    cg::CreateMovMatrix(i, 0, miny, 0);
    the_floor.ApplyTransformation(i);
  }

  // Finally, the objects are rotated so that the floor is visible.
  cg::FloatMatrix g(4, 4);
  cg::CreateXRotMatrix(g, 20);
  the_object.ApplyTransformation(g);
  the_floor.ApplyTransformation(g);

  // OpenGL setup.
  glutInit(&argc, argv);
  glutInitWindowSize(kWindowWidth, kWindowHeight);
  glutCreateWindow("Computer Graphics Coursework - s0840449");

  gluOrtho2D(-kWindowWidth/2, kWindowWidth/2,
      -static_cast<int>(kWindowHeight/2), static_cast<int>(kWindowHeight/2));

  // Callback functions
  glutDisplayFunc(display);
  glutMotionFunc(mouseDragged);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouseClicked);

  // Display everything and wait
  glutMainLoop();

  return 0;
}

//! \brief Called whenever OpenGL is redrawing the screen.
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  std::vector<cg::Vertex> points;
  shading_algorithm->Shade(the_object, the_floor, window_info, light, view,
      points, spherical_texture_map);

  if (aa) {

    std::vector<std::vector<std::vector<float> > > normal;
    normal.resize(kWindowWidth + 1);
    for (int i = 0; i <= kWindowWidth; i++) {
      normal[i].resize(kWindowHeight + 1);
      for (int j = 0; j <= kWindowHeight; j++) {
        normal[i][j].resize(3);
      }
    }

    std::vector<std::vector<std::vector<float> > > right;
    right.resize(kWindowWidth + 2);
    for (int i = 0; i <= kWindowWidth + 1; i++) {
      right[i].resize(kWindowHeight + 1);
      for (int j = 0; j <= kWindowHeight; j++) {
        right[i][j].resize(3);
      }
    }

    std::vector<std::vector<std::vector<float> > > down;
    down.resize(kWindowWidth + 1);
    for (int i = 0; i <= kWindowWidth; i++) {
      down[i].resize(kWindowHeight + 2);
      for (int j = 0; j <= kWindowHeight + 1; j++) {
        down[i][j].resize(3);
      }
    }

    std::vector<std::vector<std::vector<float> > > left;
    left.resize(kWindowWidth + 1);
    for (int i = 0; i <= kWindowWidth; i++) {
      left[i].resize(kWindowHeight + 1);
      for (int j = 0; j <= kWindowHeight; j++) {
        left[i][j].resize(3);
      }
    }

    std::vector<std::vector<std::vector<float> > > up;
    up.resize(kWindowWidth + 1);
    for (int i = 0; i <= kWindowWidth; i++) {
      up[i].resize(kWindowHeight + 1);
      for (int j = 0; j <= kWindowHeight; j++) {
        up[i][j].resize(3);
      }
    }

    for (std::vector<cg::Vertex>::iterator it = points.begin();
        it != points.end(); it++) {
      int x = (*it)[0] + kWindowWidth / 2;
      int y = (*it)[1] + kWindowHeight / 2;

      normal[x][y][0] = (*it).red();
      normal[x][y][1] = (*it).green();
      normal[x][y][2] = (*it).blue();

      right[x + 1][y][0] = (*it).red();
      right[x + 1][y][1] = (*it).green();
      right[x + 1][y][2] = (*it).blue();

      down[x][y + 1][0] = (*it).red();
      down[x][y + 1][1] = (*it).green();
      down[x][y + 1][2] = (*it).blue();

      if (x > 0) {
        left[x - 1][y][0] = (*it).red();
        left[x - 1][y][1] = (*it).green();
        left[x - 1][y][2] = (*it).blue();
      }

      if (y > 0) {
        up[x][y - 1][0] = (*it).red();
        up[x][y - 1][1] = (*it).green();
        up[x][y - 1][2] = (*it).blue();
      }
    }

    glBegin(GL_POINTS);
    for (int x = 0; x <= kWindowWidth; x++) {
      for (int y = 0; y <= kWindowHeight; y++) {
        float r;
        float g;
        float b;
        if (aa) {
          r = (normal[x][y][0] + right[x][y][0] + down[x][y][0] + left[x][y][0] + up[x][y][0]) / 5.0f;
          g = (normal[x][y][1] + right[x][y][1] + down[x][y][1] + left[x][y][1] + up[x][y][1]) / 5.0f;
          b = (normal[x][y][2] + right[x][y][2] + down[x][y][2] + left[x][y][2] + up[x][y][2]) / 5.0f;
        } else {
          r = normal[x][y][0];
          g = normal[x][y][1];
          b = normal[x][y][2];
        }

        glColor3f(r, g, b);
        glVertex2i(x - kWindowWidth / 2, y - kWindowHeight / 2);
      }
    }
    glEnd();
  } else {
    glBegin(GL_POINTS);
    for (std::vector<cg::Vertex>::iterator it = points.begin();
        it != points.end(); it++) {
      int x = (*it)[0];
      int y = (*it)[1];

      float r = (*it).red();
      float g = (*it).green();
      float b = (*it).blue();

      glColor3f(r, g, b);
      glVertex2i(x, y);
    }

    glEnd();
  }

  glFlush();
}

//! Called when the user hits a keyboard key.
void keyboard(unsigned char key, int x, int y) {
  cg::FloatMatrix m(4, 4);
  switch (key) {
    // Move the object left, up, down, or right.
    case 'a':
      cg::CreateMovMatrix(m, -5, 0, 0);
      the_object.ApplyTransformation(m);
      break;
    case 'w':
      cg::CreateMovMatrix(m, 0, 5, 0);
      the_object.ApplyTransformation(m);
      break;
    case 'd':
      cg::CreateMovMatrix(m, 5, 0, 0);
      the_object.ApplyTransformation(m);
      break;
    case 's':
      cg::CreateMovMatrix(m, 0, -5, 0);
      the_object.ApplyTransformation(m);
      break;

      // Rotate the object left, up, down, or right.
    case 'j':
      cg::CreateYRotMatrix(m, -5);
      the_object.ApplyTransformation(m);
      break;
    case 'i':
      cg::CreateXRotMatrix(m, -5);
      the_object.ApplyTransformation(m);
      break;
    case 'l':
      cg::CreateYRotMatrix(m, 5);
      the_object.ApplyTransformation(m);
      break;
    case 'k':
      cg::CreateXRotMatrix(m, 5);
      the_object.ApplyTransformation(m);
      break;

      // Scale the object up and down.
    case '+':
      cg::CreateScaleMatrix(m, 1.1f, 1.1f, 1.1f);
      the_object.ApplyTransformation(m);
      break;
    case '-':
      cg::CreateScaleMatrix(m, 0.9f, 0.9f, 0.9f);
      the_object.ApplyTransformation(m);
      break;

      // Increase/decrease k constants.
    case 'r':
      shading_algorithm->k_a() += 0.1f;
      break;
    case 'f':
      if (shading_algorithm->k_a() >= 0.1f) {
        shading_algorithm->k_a() -= 0.1f;
      }
      break;
    case 't':
      shading_algorithm->k_d() += 0.1f;
      break;
    case 'g':
      if (shading_algorithm->k_d() >= 0.1f) {
        shading_algorithm->k_d() -= 0.1f;
      }
      break;
    case 'y':
      shading_algorithm->k_s() += 0.1f;
      break;
    case 'h':
      if (shading_algorithm->k_s() >= 0.1f) {
        shading_algorithm->k_s() -= 0.1f;
      }
      break;

      // Increase/decrease i constants.
    case 'p':
      shading_algorithm->i_a() += 0.1f;
      break;
    case ';':
      if (shading_algorithm->i_a() >= 0.1f) {
        shading_algorithm->i_a() -= 0.1f;
      }
      break;
    case '[':
      shading_algorithm->i_d() += 0.1f;
      break;
    case '\'':
      if (shading_algorithm->i_d() >= 0.1f) {
        shading_algorithm->i_d() -= 0.1f;
      }
      break;
    case ']':
      shading_algorithm->i_s() += 0.1f;
      break;
    case '#':
      if (shading_algorithm->i_s() >= 0.1f) {
        shading_algorithm->i_s() -= 0.1f;
      }
      break;

      // Increase/decrease the alpha constant.
    case '\\':
      shading_algorithm->alpha() += 5.0f;
      break;
    case 'z':
      if (shading_algorithm->alpha() >= 5.0f) {
        shading_algorithm->alpha() -= 5.0f;
      }
      break;

      // Turn anti-aliasing on/off.
    case '/':
      aa = (aa) ? false : true;
      break;

      // Turn shadows on/off.
    case '.':
      shading_algorithm->ToggleShadows();
      break;

      // Increase/decrease RGB.
    case 'x':
      if (shading_algorithm->red_strength() <= 0.9f) {
        shading_algorithm->red_strength() += 0.1f;
      }
      break;
    case 'c':
      if (shading_algorithm->red_strength() >= 0.1f) {
        shading_algorithm->red_strength() -= 0.1f;
      }
      break;
    case 'v':
      if (shading_algorithm->green_strength() <= 0.9f) {
        shading_algorithm->green_strength() += 0.1f;
      }
      break;
    case 'b':
      if (shading_algorithm->green_strength() >= 0.1f) {
        shading_algorithm->green_strength() -= 0.1f;
      }
      break;
    case 'n':
      if (shading_algorithm->blue_strength() <= 0.9f) {
        shading_algorithm->blue_strength() += 0.1f;
      }
      break;
    case 'm':
      if (shading_algorithm->blue_strength() >= 0.1f) {
        shading_algorithm->blue_strength() -= 0.1f;
      }
      break;

    default:
      return;
  }

  glutPostRedisplay();
}

//! Called when the user moves the mouse with a key held down.
void mouseDragged(int x, int y) {
  // Take care of the very first mouse movement in a drag.
  if (!old_mouse_location.set()) {
    old_mouse_location.set_x(x);
    old_mouse_location.set_y(y);
    return;
  }

  float dx = x - old_mouse_location.x();
  float dy = y - old_mouse_location.y();

  // Only recalculate when we move the mouse far enough.
  if (abs(dx) < 5 && abs(dy) < 5) {
    return;
  }

  if (current_button == GLUT_LEFT_BUTTON) {
    cg::FloatMatrix a(4, 4);
    cg::FloatMatrix b(4, 4);

    // Rotate in the Y axis for left/right, the X axis for up/down.
    cg::CreateYRotMatrix(a, dx);
    cg::CreateXRotMatrix(b, dy);

    the_object.ApplyTransformation(a);
    the_object.ApplyTransformation(b);
  } else if (current_button == GLUT_RIGHT_BUTTON) {
    cg::FloatMatrix a(4, 4);

    cg::CreateMovMatrix(a, dx, -dy, 0);
    the_object.ApplyTransformation(a);
  }

  old_mouse_location.set_x(x);
  old_mouse_location.set_y(y);

  glutPostRedisplay();
}

void mouseClicked(int button, int state, int x, int y) {
  if (state == GLUT_UP) {
    old_mouse_location.clear();
    return;
  }

  if (button < 3) {
    current_button = button;
    return;
  }

  // The below applies to scrolling with the mouse wheel.
  cg::FloatMatrix m(4, 4);
  bool changed = false;

  if (button == 3) {
    // Although there is no constant for it, 3 is 'scroll-up'
    cg::CreateScaleMatrix(m, 1.1, 1.1, 1.1);
    changed = true;
  } else if (button == 4) {
    // Although there is no constant for it, 4 is 'scroll-down'
    cg::CreateScaleMatrix(m, 0.9, 0.9, 0.9);
    changed = true;
  }

  if (changed) {
    the_object.ApplyTransformation(m);
    glutPostRedisplay();
  }
}
