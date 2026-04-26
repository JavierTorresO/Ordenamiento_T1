#!/bin/bash

# Compilar
make clean && make

N=1000000
K=4
GRAN=4096
REPS=1

run_perf_term() {
    LABEL=$1
    MODE=$2
    THREADS=$3
    
    echo ""
    echo "=========================================================="
    echo " ANALIZANDO: $LABEL (Threads: $THREADS)"
    echo "=========================================================="
    export OMP_NUM_THREADS=$THREADS
    
    # Ejecución estándar de perf
    LC_NUMERIC=C perf stat -e cycles,instructions,cache-references,cache-misses ./sort $N $K $GRAN $REPS $MODE
}

run_perf_term "Mergesort Secuencial" 1 1
run_perf_term "K-Way Secuencial" 2 1
run_perf_term "Mergesort Paralelo" 4 8
run_perf_term "K-Way Paralelo Completo" 6 8

echo ""
echo "----------------------------------------------------------"
echo "Análisis de terminal finalizado."
echo "----------------------------------------------------------"
