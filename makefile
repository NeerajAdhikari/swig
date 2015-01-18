all:
	g++ -o test src/test.cc src/Matrix.cpp src/TfMatrix.cpp\
	 src/VectorTriplet.cpp -Iinclude/ -lSDL --std=c++11
