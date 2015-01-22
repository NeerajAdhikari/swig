#include "Drawer.h"
#include "Linspace.h"

// Construct.
Drawer::Drawer(Plotter_ *pltr)
    : plotter(pltr),
    depth({pltr->width(),pltr->height()}) {}

    // Clear scren with black
    void Drawer::clear() {
        plotter->clear();
        // Also clear the depth-buffer
        depth.clear();
    }

// Update the screen
void Drawer::update() {
    plotter->update();
}

// Draw line between start and end
// using Bresenham's famous algorithm
void Drawer::line(const ScreenPoint& start,
        const ScreenPoint& end) {
    int dx, dy, x, y, xc, yc;
    int D,twodydx,twodxdy,twody,twodx;

    // If they aren't cast to int then the result will
    // be a uint which will never have a negative value
    dx = Math::abs((int)end.x-(int)start.x);
    dy = Math::abs((int)end.y-(int)start.y);
    if (dx==0) xc=0;
    else xc = (start.x>end.x)?-1:1;
    if (dy==0) yc=0;
    else yc = (start.y>end.y)?-1:1;

    x = start.x;
    y = start.y;
    plotter->plot(x,y,start.color);

    if (dx>dy) {
        D = 2*dy-dx;
        twodydx = 2*(dy-dx);
        twody = 2*dy;

        while (x!=end.x) {
            x+=xc;
            if (D>0) {
                y+=yc;
                plotter->plot(x,y,start.color);
                D += twodydx;
            } else {
                plotter->plot(x,y,start.color);
                D += twody;
            }
        }

    } else {
        D = 2*dx-dy;
        twodxdy = 2*(dx-dy);
        twodx = 2*dx;

        while (y!=end.y) {
            y+=yc;
            if (D>0) {
                x+=xc;
                plotter->plot(x,y,start.color);
                D += twodxdy;
            } else {
                plotter->plot(x,y,start.color);
                D += twodx;
            }
        }
    }
}

// Draw a horizontal line between (xs,y) and (xe,y)
// This one doesn't consider the point depths.
void Drawer::hLine(int y, int xStart, int xEnd, Color cl) {
    if (xStart>xEnd) {
        swap(xStart,xEnd);
    }

    if( y >= (int)plotter->height() || y < 0)
        return;
    if( xStart >= (int)plotter->width() || xEnd < 0)
        return;
    xStart = Math::max(0,xStart);
    xEnd = Math::min(xEnd,(int)plotter->width()-1);

    while(xStart <= xEnd){
        plotter->plot(xStart,y,cl,true);
        xStart++;
    }
}

// This one considers the pixel depths while plotting. It only
// plots points closer than already there.
// The parameters are : y-coordinate, starting x-coordinate,
// starting depth value, ending x-coordinate and ending depth
// value
void Drawer::hLineD(int y, int xStart,
        int dStart, int xEnd, int dEnd, Color cl) {
    // Sort the start end end values if they are not in order
    if (xStart>xEnd) {
        swap(xStart,xEnd);
        swap(dStart,dEnd);
    }

    if( y >= (int)plotter->height() || y < 0)
        return;
    if( xStart >= (int)plotter->width() || xEnd < 0)
        return;
    xStart = Math::max(0,xStart);
    xEnd = Math::min(xEnd,(int)plotter->width()-1);

    int space = xEnd-xStart+1;
    Linspace d(dStart,dEnd,space);
    while(xStart <= xEnd){
        // TODO: have some limit to depth ie
        // viewable from (0 to 1) * someconstant
        if (d>=depth(xStart,y)) {
            plotter->plot(xStart,y,cl,true);
            depth(xStart,y)=d;
        }
        ++xStart;
        ++d;
    }

}


// We need to sort the points according to their
// y-coordinates
void Drawer::initAscending(ScreenPoint& start, ScreenPoint& mid, ScreenPoint& end,
        const ScreenPoint& pt1, const ScreenPoint& pt2, const ScreenPoint& pt3){

    if (pt1.y<=pt2.y && pt1.y<=pt3.y) {
        start = pt1;
        if (pt2.y<=pt3.y) {
            mid = pt2;
            end = pt3;
        } else {
            mid = pt3;
            end = pt2;
        }
    }
    else if (pt2.y<=pt1.y && pt2.y<=pt3.y) {
        start = pt2;
        if (pt1.y<=pt3.y) {
            mid = pt1;
            end = pt3;
        } else {
            mid = pt3;
            end = pt1;
        }
    }
    else {
        start = pt3;
        if (pt1.y<=pt2.y) {
            mid = pt1;
            end = pt2;
        } else {
            mid = pt2;
            end = pt1;
        }
    }

}



// Fill the triangle bounded by pt1, pt2 and pt3
// What is implemented here is a special case of
// scan-line filling which works only for triangles.
void Drawer::fill(ScreenPoint pt1, ScreenPoint pt2,
        ScreenPoint pt3, Color fillcolor) {

    ScreenPoint start, mid, end;
    initAscending(start,mid,end,pt1,pt2,pt3);

    if(start.y == end.y)
        return;
    if( start.y >= (int)plotter->height() || end.y < 0)
        return;

    Linspace x1(start.x,mid.x, mid.y-start.y+1);
    Linspace x2(start.x,end.x, end.y-start.y+1);

    for(int i=start.y;i<mid.y;i++){
        if( i < (int)plotter->height() && i >= 0)
            hLine(i,x1,x2,fillcolor);
        ++x1;
        ++x2;
    }

    Linspace x3(mid.x,end.x, end.y-mid.y+1);
    Linspace d3(mid.d,end.d, end.y-mid.y+1);
    for(int i=mid.y;i<=end.y;i++){
        if( i < (int)plotter->height() && i >= 0)
            hLine(i, x2, x3, fillcolor);
        ++x2;
        ++x3;
    }
}


// Fill the triangle bounded by pt1, pt2 and pt3
// What is implemented here is a special case of
// scan-line filling which works only for triangles.
// considering depth buffer
void Drawer::fillD(ScreenPoint pt1, ScreenPoint pt2,
        ScreenPoint pt3, Color fillcolor) {

    ScreenPoint start, mid, end;
    initAscending(start,mid,end,pt1,pt2,pt3);

    if(start.y == end.y)
        return;
    if( start.y >= (int)plotter->height() || end.y < 0)
        return;

    Linspace x1(start.x,mid.x, mid.y-start.y+1);
    Linspace d1(start.d,mid.d, mid.y-start.y+1);

    Linspace x2(start.x,end.x, end.y-start.y+1);
    Linspace d2(start.d,end.d, end.y-start.y+1);


    for(int i=start.y;i<mid.y;i++){
        if( i < (int)plotter->height() && i >= 0)
            hLineD(i,x1,d1,x2,d2,fillcolor);
        ++x1;
        ++x2;
        ++d1;
        ++d2;
    }

    Linspace x3(mid.x,end.x, end.y-mid.y+1);
    Linspace d3(mid.d,end.d, end.y-mid.y+1);
    for(int i=mid.y;i<=end.y;i++){
        if( i < (int)plotter->height() && i >= 0)
            hLineD(i, x2, d2, x3, d3, fillcolor);
        ++x2;
        ++x3;
        ++d2;
        ++d3;
    }
}
