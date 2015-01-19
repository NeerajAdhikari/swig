all:
	g++ -o bin/test test.cc src/Matrix.cpp src/TfMatrix.cpp\
	 src/VectorTriplet.cpp -Iinclude/ -lSDL2 --std=c++11

other:
	g++ -o bin/other test.cpp src/Matrix.cpp src/TfMatrix.cpp\
	 src/VectorTriplet.cpp -Iinclude/ -lSDL2 --std=c++11
