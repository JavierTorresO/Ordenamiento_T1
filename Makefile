CXX = g++
CXXFLAGS = -O3 -fopenmp -I.

SRC = main.cpp utils/utils.cpp algoritmos/mergesort.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o sort

clean:
	rm -f sort