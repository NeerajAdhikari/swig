all:
	g++ -o bin/test test.cc src/Matrix.cpp src/TfMatrix.cpp\
	 src/VectorTriplet.cpp -Iinclude/ -lSDL2 -w --std=c++11
