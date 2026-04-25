CXX = g++
CXXFLAGS = -O3 -fopenmp -I.
TARGET = sort
VALIDATOR_TARGET = validator

# Archivos comunes de algoritmos y utilidades
COMMON_SRC = utils/utils.cpp \
	algoritmos/mergesort.cpp \
	algoritmos/kway.cpp \
	paralelismo/merge_paralelo.cpp \
	paralelismo/mergesort_paralelo.cpp \
	paralelismo/kway_paralelo.cpp

SRC = main.cpp $(COMMON_SRC)
VALIDATOR_SRC = validator.cpp $(COMMON_SRC)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Compila el ejecutable del validador
$(VALIDATOR_TARGET): $(VALIDATOR_SRC)
	$(CXX) $(CXXFLAGS) $(VALIDATOR_SRC) -o $(VALIDATOR_TARGET)

# Comando para compilar y ejecutar el validador
validate: $(VALIDATOR_TARGET)
	./$(VALIDATOR_TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe $(VALIDATOR_TARGET) $(VALIDATOR_TARGET).exe

.PHONY: all validate clean
