//! \author Stephen McGruer

#include "./mouse_loc.h"

namespace computer_graphics {

void MouseLoc::set_x(int x) {
  x_ = x;
  set_ = true;
}

void MouseLoc::set_y(int y) {
  y_ = y;
  set_ = true;
}

void MouseLoc::clear() {
  set_ = false;
}
}
