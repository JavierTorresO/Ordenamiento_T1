CXX = g++
CXXFLAGS = -O3 -fopenmp -I.

SRC = main.cpp \
	utils/utils.cpp \
	algoritmos/mergesort.cpp \
	algoritmos/kway.cpp \
	paralelismo/merge_paralelo.cpp \
	paralelismo/mergesort_paralelo.cpp

TARGET = sort
SRC = main.cpp utils/utils.cpp algoritmos/mergesort.cpp algoritmos/kway.cpp paralelismo/kway_paralelo.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
