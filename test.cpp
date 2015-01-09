#include <iostream>
#include "Matrix.h"
#include "Object.h"

// TODO in Matrix class, check if we are performing action
// on same matrix
// TODO vectors have 4th component zero and point have 4th component 1

int main() {
    Matrix r = Matrix::rotation(45,{1,0,0},{0,0,0});
    r.print();
    r = Matrix::rotation(45,{0,1,0},{0,0,0});
    r.print();
    r = Matrix::rotation(45,{0,0,1},{0,0,0});
    r.print();
    Object e(1);
    e.setVertex(0,{10,20,30});
    e.vertex() %= r;
    e.vertex().print();
    return 0;

    // Testing composite transformation
    Matrix n = Matrix::translation({10,20,30}) * Matrix::scaling({10,2,11});
    n.print();
    return 0;

    // Testing object
    Object o(4);
    o.setVertex(0,{0,0,0});
    o.setVertex(1,{100,150,100});
    o.setVertex(2,{50,100,120});
    o.setVertex(3,{80,90,10});
    o.vertex() %= Matrix::translation({10.0,20.0,30.0});
    o.vertex() %= Matrix::scaling({1,2,3});
    o.vertex().print();
    return 0;

    // Testing normalized() vector
    Triplet<float> f = {1,1,1};
    Triplet<float> g = f.normalized();
    std::cout << g.x << " " << g.y << " " << g.z << std::endl;
    return 0;

}
