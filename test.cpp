#include <iostream>
#include "Matrix.h"
#include "TransformationMatrix.h"
#include "Object.h"

int main() {
    Matrix r = TransformationMatrix::rotation(45,{1,0,0},{0,0,0});
    //r = TransformationMatrix::rotation(45,{0,1,0},{0,0,0});
    //r = TransformationMatrix::rotation(45,{0,0,1},{0,0,0});
    Object e(1);
    e.setVertex(0,{10,20,30});
    e.vertex() /= r;
    e.vertex().print();
    return 0;

    // Testing composite transformation
    Matrix n = TransformationMatrix::translation({10,20,30}) * TransformationMatrix::scaling({10,2,11},{0,0,0});
    n.print();
    return 0;

    // Testing object
    Object o(4);
    o.setVertex(0,{0,0,0});
    o.setVertex(1,{100,150,100});
    o.setVertex(2,{50,100,120});
    o.setVertex(3,{80,90,10});
    o.vertex() /= TransformationMatrix::translation({10.0,20.0,30.0});
    o.vertex() /= TransformationMatrix::scaling({1,2,3},{0,0,0});
    o.vertex().print();
    return 0;

    // Testing normalized() vector
    VectorTriplet f = {1,1,1};
    VectorTriplet g = f.normalized();
    std::cout << g.x << " " << g.y << " " << g.z << std::endl;
    return 0;

}
