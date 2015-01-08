#include <iostream>
#include "Matrix.h"
#include "Object.h"

// TODO in Matrix class, check if we are performing action
// on same matrix, multiplication will fail

int main() {
    Object o(4);
    o.setVertex(0,{0,0,0});
    o.setVertex(1,{100,150,100});
    o.setVertex(2,{50,100,120});
    o.setVertex(3,{80,90,10});
    /*
    o.vertex().initialize(
            0,100,50,80,
            0,150,100,90,
            0,100,120,10,
            1,1,1,1
            );
   */
    o.vertex() %= Matrix::translation({10.0,20.0,30.0});
    o.vertex() %= Matrix::scaling({1,2,3});
    o.vertex().print();
    return 0;
}
