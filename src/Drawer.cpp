#include "Drawer.h"

Drawer::Drawer(Plotter_ *pltr) : plotter(pltr) {}

// Clear scren with black
void Drawer::clear() {
    plotter->clear();
}

// Update the screen
void Drawer::update() {
    plotter->update();
}

// Draw line between start and end
void Drawer::line(const ScreenPoint& start,
        const ScreenPoint& end) {
    int dx, dy, x, y, xc, yc;
    int D,twodydx,twodxdy,twody,twodx;

    // If they aren't cast to int then the result will
    // be an uint which will never have a negative value
    dx = abs((int)end.x-(int)start.x);
    dy = abs((int)end.y-(int)start.y);
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

// Fill the triangle bounded by pt1, pt2 and pt3
void Drawer::fill(ScreenPoint pt1, ScreenPoint pt2,
        ScreenPoint pt3, Color fillcolor) {
    ScreenPoint start, mid, end;
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

    if (start.y==end.y) {
         hLine(start.y,start.x,mid.x,fillcolor);
         hLine(start.y,mid.x,end.x,fillcolor);
         return;
    }
    float xda,xdb,r1,r2,r3;
    r2=((float)end.x-start.x)/(end.y-(float)start.y);
    if (start.y!=mid.y) {
        r1=((float)mid.x-start.x)/((float)mid.y-start.y);
        for (auto i=start.y; i<=mid.y; i++) {
            xda = r1*(i-start.y);
            xdb = r2*(i-start.y);
            hLine(i, start.x+xda, start.x+xdb, fillcolor);
        }
    }
    if (mid.y!=end.y) {
        r3=((float)end.x-mid.x)/(end.y-(float)mid.y);
        for (auto i=mid.y; i<=end.y; i++) {
            xda = r3*(i-mid.y);
            xdb = r2*(i-start.y);
            hLine(i, mid.x+xda, start.x+xdb, fillcolor);
        }
    }
}

// Draw a horizontal line between (xs,y) and (xe,y)
void Drawer::hLine(unsigned y, unsigned xs, unsigned xe, Color cl) {
    if (xs>xe) {
        auto t = xs;
        xs = xe;
        xe = t;
    }
    for (auto i=0; i<=(xe-xs); i++) {
        plotter->plot(xs+i,y,cl,true);
    }
}

