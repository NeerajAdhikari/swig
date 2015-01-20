#include "Drawer.h"

Drawer::Drawer(Plotter_ *pltr) : plotter(pltr),
    depth({pltr->width(),pltr->height()}) {}

// Clear scren with black
void Drawer::clear() {
    plotter->clear();
    depth.clear();
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
         //hLine(start.y,start.x,mid.x,fillcolor);
         hLineD(start.y,start.x,start.d,mid.x,mid.d,fillcolor);
         //hLine(start.y,mid.x,end.x,fillcolor);
         hLineD(start.y,mid.x,mid.d,end.x,end.d,fillcolor);
         return;
    }

    int ctr1,ctr2,ctr3,x1,x2,x3;
    int dctr1,dctr2,dctr3,d1,d2,d3;
    ctr1=0;ctr2=0;x1=start.x;x2=start.x;
    dctr1=0;dctr2=0;d1=start.d;d2=start.d;
    if (start.y!=mid.y) {
        for (auto i=start.y; i<=mid.y; i++) {
            hLineD(i, x1, d1, x2, d2, fillcolor);
            ctr1+=(mid.x-start.x);
            if (abs(ctr1)>=(mid.y-start.y)) {
                x1+=ctr1/(mid.y-start.y);
                ctr1%=mid.y-start.y;
            }
            ctr2+=(end.x-start.x);
            if (abs(ctr2)>=(end.y-start.y)) {
                x2+=ctr2/(end.y-start.y);
                ctr2%=end.y-start.y;
            }
            dctr1+=(mid.d-start.d);
            if (abs(dctr1)>=(mid.y-start.y)) {
                d1+=dctr1/(mid.y-start.y);
                dctr1%=mid.y-start.y;
            }
            dctr2+=(end.d-start.d);
            if (abs(dctr2)>=(end.y-start.y)) {
                d2+=dctr2/(end.y-start.y);
                dctr2%=end.y-start.y;
            }
        }
    }
    ctr3=0;x3=mid.x;
    dctr3=0;d3=mid.d;
    if (mid.y!=end.y) {
        for (auto i=mid.y; i<=end.y; i++) {
            hLineD(i, x2, d2, x3, d3, fillcolor);
            ctr3+=(end.x-mid.x);
            if (abs(ctr3)>=(end.y-mid.y)) {
                x3+=ctr3/(end.y-mid.y);
                ctr3%=end.y-mid.y;
            }
            ctr2+=(end.x-start.x);
            if (abs(ctr2)>=(end.y-start.y)) {
                x2+=ctr2/(end.y-start.y);
                ctr2%=end.y-start.y;
            }
            dctr3+=(end.d-mid.d);
            if (abs(dctr3)>=(end.y-mid.y)) {
                d3+=dctr3/(end.y-mid.y);
                dctr3%=end.y-mid.y;
            }
            dctr2+=(end.d-start.d);
            if (abs(dctr2)>=(end.y-start.y)) {
                d2+=dctr2/(end.y-start.y);
                dctr2%=end.y-start.y;
            }
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

// Draw a horizontal line between (xs,y) and (xe,y)
void Drawer::hLineD(unsigned y, unsigned xs,
        unsigned hs, unsigned xe, unsigned he, Color cl) {
    if (xs>xe) {
        auto t = xs; xs = xe; xe = t;
        t = hs; hs = he; he = t;
    }
    int dx=xe-xs, dh=he-hs,h=hs,ctr=0;
    for (auto i=1; i<=(xe-xs); i++) {
        if (h>=depth(xs+i,y)) {
            plotter->plot(xs+i,y,cl,true);
            depth(xs+i,y)=h;
        }
        ctr += dh;
        if (abs(ctr)>=dx) {
            h += ctr/dx;
            ctr %= dx;
        }
    }
}
