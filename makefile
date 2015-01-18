all:
	g++ -o test test.cc src/Matrix.cpp src/TfMatrix.cpp\
	 src/VectorTriplet.cpp -Iinclude/ -lSDL2 --std=c++11
