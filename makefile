all:
	g++ -o test test.cc Matrix.cpp TfMatrix.cpp VectorTriplet.cpp\
	     -lSDL --std=c++11
