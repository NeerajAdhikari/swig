#ifndef __DRAWER__
#define __DRAWER__

#include "helper.h"
#include "Point.h"
#include "setPlotter.h"
#include "Matrix.h"

#include SSTR(Plotter_.h)

// The drawer class is an abstraction that handles the drawing of
// primitives. Drawing lines, filling polygons, etc are done
// through Drawer.
class Drawer {

    private:
    // Pointer to a plotter object
    Plotter_ *plotter;

    // A depth buffer, a matrix of uint32_t
    Matrix<uint32_t> depth;

    void initAscending(ScreenPoint& start, ScreenPoint& mid, ScreenPoint& end,
            const ScreenPoint& pt1, const ScreenPoint& pt2, const ScreenPoint& pt3);

    public:
    Drawer(Plotter_ *pltr);

    // Update screen.
    void update();

    // Clear the screen
    void clear();

    // Draw a line from start to end
    void line(const ScreenPoint& start,const ScreenPoint& end);

    // Draw a horizontal line between (xs,y) and (xe,y)
    void hLine(int y, int xs, int xe, Color cl);
    // Draw a horizontal line considering the depth buffer
    void hLineD(int y, int xs, int hs,
            int xe, int he, Color cl);

    // Fill the triangle bounded by pt1, pt2 and pt3
    void fill(ScreenPoint pt1, ScreenPoint pt2,
            ScreenPoint pt3, Color fillcolor);
    // Fill the triangle bounded by pt1, pt2 and pt3
    // considering depth buffer
    void fillD(ScreenPoint pt1, ScreenPoint pt2,
            ScreenPoint pt3, Color fillcolor);

};

#endif
