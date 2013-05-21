//! \author Stephen McGruer

#ifndef SRC_MOUSELOC_H_
#define SRC_MOUSELOC_H_

namespace computer_graphics {

/** \class MouseLoc
    \brief Represents a mouse location.

    Tracks whether or not the location has been 'set' - that is,
    whether or not the (x,y) values have been explicitly set.
 */
class MouseLoc {
  public:
    MouseLoc() : x_(0), y_(0), set_(false) {
    }

    MouseLoc(int x, int y) : x_(x), y_(y), set_(true) {
    }

    inline int x() const { return x_; }
    inline int y() const { return y_; }
    inline bool set() const { return set_; }

    // Cannot just use mutator methods, as need to change mset.
    void set_x(int x);
    void set_y(int y);
    void clear();
  private:
    int x_;
    int y_;
    bool set_;
};
}

#endif  // SRC_MOUSELOC_H_
