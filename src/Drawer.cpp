#include "Drawer.h"

// Construct.
Drawer::Drawer(Plotter_ *pltr) : plotter(pltr),
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
// What is implemented here is a special case of
// scan-line filling which works only for triangles.
void Drawer::fill(ScreenPoint pt1, ScreenPoint pt2,
        ScreenPoint pt3, Color fillcolor) {

    ScreenPoint start, mid, end;
    // First we need to sort the points according to their
    // y-coordinates
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

    // If start and have the same y-value, then the triangle
    // degenerates into a line. Plot it.
    if (start.y==end.y) {
         //hLine(start.y,start.x,mid.x,fillcolor);
         hLineD(start.y,start.x,start.d,mid.x,mid.d,fillcolor);
         //hLine(start.y,mid.x,end.x,fillcolor);
         hLineD(start.y,mid.x,mid.d,end.x,end.d,fillcolor);
         return;
    }

    // The algorithm begins.
    // The reason the algorithm looks a bit complex is that
    // floating-point calculations have been dispensed with
    // and have been implemented using integers only.

    // The three counters are for determining when to change
    // x as y changes. The three x-values are for the three
    // sides of the triangle.
    int ctr1,ctr2,ctr3,x1,x2,x3;
    // The three depth-counters are for determining when to change
    // d (depth) as y changes. The three d-values are for the three
    // sides of the triangle. Depth need to be taken into account
    // because we're doing depth-buffering.
    int dctr1,dctr2,dctr3,d1,d2,d3;
    // What we are doing here is evaluating the following
    // equation : delta_x = (dx/dy)*delta_y
    // delta_x will be 1, as we're traversing over horizontal
    // scan-lines. We initialize a counter to 0, and then add dy
    // until counter>=dy. At that point, we add counter/dy to x
    // and decrease counter (counter = counter%dy). Then we
    // repeat the process. So basically we're doing division in
    // integer terms.

    // Initialize all values. In the first part, we will fill till
    // y reaches mid.y. x1 and d1 are for the start-mid side and
    // x2 and d2 are for the start-end side.
    ctr1=0;ctr2=0;x1=start.x;x2=start.x;
    dctr1=0;dctr2=0;d1=start.d;d2=start.d;

    if (start.y!=mid.y) {
        for (auto i=start.y; i<=mid.y; i++) {
            // Plot a horizontal line
            hLineD(i, x1, d1, x2, d2, fillcolor);
            // Add to counter
            ctr1+=(mid.x-start.x);
            // If it exceeds dy, increment x and readjust
            if (abs(ctr1)>=(mid.y-start.y)) {
                x1+=ctr1/(mid.y-start.y);
                ctr1%=mid.y-start.y;
            }
            // Add to counter
            ctr2+=(end.x-start.x);
            // If it exceeds dy, increment x and readjust
            if (abs(ctr2)>=(end.y-start.y)) {
                x2+=ctr2/(end.y-start.y);
                ctr2%=end.y-start.y;
            }
            // Add to counter
            dctr1+=(mid.d-start.d);
            // If it exceeds dy, increment d and readjust
            if (abs(dctr1)>=(mid.y-start.y)) {
                d1+=dctr1/(mid.y-start.y);
                dctr1%=mid.y-start.y;
            }
            // Add to counter
            dctr2+=(end.d-start.d);
            // If it exceeds dy, increment d and readjust
            if (abs(dctr2)>=(end.y-start.y)) {
                d2+=dctr2/(end.y-start.y);
                dctr2%=end.y-start.y;
            }
        }
    }
    // Initialize counters, x and d for the mid-end side
    // We retain values of ctr2, dctr2, x2 and d2 as the
    // start-end side of the triangle hasn't finished.
    ctr3=0;x3=mid.x;
    dctr3=0;d3=mid.d;
    // Now we'll plot from mid.y to end.y
    if (mid.y!=end.y) {
        for (auto i=mid.y; i<=end.y; i++) {
            // Draw a horizontal line
            hLineD(i, x2, d2, x3, d3, fillcolor);
            // Add to counter
            ctr3+=(end.x-mid.x);
            // If it exceeds dy, increment x and readjust
            if (abs(ctr3)>=(end.y-mid.y)) {
                x3+=ctr3/(end.y-mid.y);
                ctr3%=end.y-mid.y;
            }
            // Add to counter
            ctr2+=(end.x-start.x);
            // If it exceeds dy, increment x and readjust
            if (abs(ctr2)>=(end.y-start.y)) {
                x2+=ctr2/(end.y-start.y);
                ctr2%=end.y-start.y;
            }
            // Add to counter
            dctr3+=(end.d-mid.d);
            // If it exceeds dy, increment d and readjust
            if (abs(dctr3)>=(end.y-mid.y)) {
                d3+=dctr3/(end.y-mid.y);
                dctr3%=end.y-mid.y;
            }
            // Add to counter
            dctr2+=(end.d-start.d);
            // If it exceeds dy, increment d and readjust
            if (abs(dctr2)>=(end.y-start.y)) {
                d2+=dctr2/(end.y-start.y);
                dctr2%=end.y-start.y;
            }
        }
    }
}

// Draw a horizontal line between (xs,y) and (xe,y)
// This one doesn't consider the point depths.
void Drawer::hLine(unsigned y, unsigned xs, unsigned xe, Color cl) {
    if (xs>xe) {
        swap(xs,xe);
    }
    for (auto i=0; i<=(xe-xs); i++) {
        plotter->plot(xs+i,y,cl,true);
    }
}

// This one considers the pixel depths while plotting. It only
// plots points closer than already there.
// The parameters are : y-coordinate, starting x-coordinate,
// starting depth value, ending x-coordinate and ending depth
// value
void Drawer::hLineD(unsigned y, unsigned xStart,
        unsigned dStart, unsigned xEnd, unsigned dEnd, Color cl) {
    // Sort the start end end values if they are not in order
    if (xStart>xEnd) {
        swap(xStart,xEnd);
        swap(dStart,dEnd);
    }
    // Here too, we use integer calculations to evaluate the
    // division Dh = (dh/dx)*Dx
    int Dx=xEnd-xStart, Dd=dEnd-dStart,d=dStart,ctr=0;
    for (auto i=0; i<=(xEnd-xStart); i++) {
        if (d>=depth(xStart+i,y)) {
            plotter->plot(xStart+i,y,cl,true);
            depth(xStart+i,y)=d;
        }
        ctr += Dd;
        if (Dx && abs(ctr)>=Dx) {
            d += ctr/Dx;
            ctr %= Dx;
        }
    }
}
